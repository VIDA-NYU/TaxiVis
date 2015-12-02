#include "HeatMap.hpp"
#include "QMapView.hpp"
#include "global.h"
#include "geographicalviewwidget.h"
#include "util/colorbar.h"
#include "util/heatedobjectscale.h"
#include <QtGui/QVector2D>

HeatMap::HeatMap(GeographicalViewWidget *gw) :
    RenderingLayer(false),
    resolution(2048, 2048),
    initialized(false),
    normalized(false),
    dataReady(false),
    textureDirty(false),
    visualDirty(false),
    colorBarVisible(true),
    labelFont("Arial", 24),
    labelMetrics(labelFont),
    geoWidget(gw)
{
  this->colorScale = new HeatedObjectScale();
  this->colorScale->setReverse(true);
  this->colorBar = new ColorBar();
  this->colorBar->setColorScale(this->colorScale);
  this->colorBar->setRealMinMax(0.0, 32.0);
  this->colorBar->setUnit("Average Rides per Hour");

  this->setRegion(QRectF(40.5641,-74.0591, 0.293033, 0.335083));
  this->setPointSize(32);

  this->maxValue = 0;
}

HeatMap::~HeatMap()
{
  delete this->colorBar;
  delete this->colorScale;
}

void HeatMap::setRegion(const QRectF &rect)
{
  this->region = rect;
  this->updateGrid();
}

void HeatMap::setResolution(const QSize &size)
{
  this->resolution = size;
  this->updateGrid();
}

void HeatMap::updateGrid()
{
  this->binSize = QSizeF((this->region.width()+1e-3)/this->resolution.width(),
                         (this->region.height()+1e-3)/this->resolution.height());
  this->visualDirty = true;
  if (this->enabled && this->dataReady)
    this->computeVisualData();
}

void HeatMap::setPointSize(int size)
{
  this->pointSize = size;
  this->visualDirty = true;

  float scale = 8.0;
  float c = 64.0;
  this->pointImage = QImage(this->pointSize, this->pointSize, QImage::Format_ARGB32);
  float half = this->pointSize*0.5;
  for (int y=0; y<this->pointSize; y++) {
    QRgb *line = reinterpret_cast<QRgb*>(this->pointImage.scanLine(y));
    for (int x=0; x<this->pointSize; x++) {
      float distance = sqrt(pow(x-half, 2.0) + pow(y-half, 2.0));
      float intensity = (1.0-distance/half)*scale;
      if (intensity<0) intensity = 0;
      line[x] = QColor(c, c, c, intensity).rgba();
    }
  }
}

float HeatMap::getMaxValue()
{
  return this->maxBinCount;
}

void HeatMap::setMaxValue(float value)
{
  this->maxValue = value;
  this->textureDirty = true;
}

void HeatMap::setEnabled(bool r)
{
  if (this->enabled!=r) {
    this->enabled = r;
    if (this->enabled && this->visualDirty)
        this->computeVisualData();
  }
}

void HeatMap::setColorScale(ColorScale *cs)
{
  this->colorScale = cs;
}

ColorScale *HeatMap::getColorScale()
{
  return this->colorScale;
}

void HeatMap::setNormalized(bool n)
{
  if (this->normalized!=n) {
    this->normalized = n;
    this->textureDirty = true;
    this->updateColorBar();
  }
}

bool HeatMap::isNormalized()
{
  return this->normalized;
}

void HeatMap::showColorBar(bool b)
{
  if (this->colorBarVisible!=b) {
    this->colorBarVisible = b;
  }
}

bool HeatMap::isColorBarVisible()
{
  return this->colorBarVisible;
}

void HeatMap::updateColorBar()
{
  if (this->normalized) {
    this->colorBar->setRealMinMax(0.0, this->maxValue);
    this->colorBar->setUnit("Total Number of Rides");
  }
  else {
    this->colorBar->setRealMinMax(0.0, 32.0);
    this->colorBar->setUnit("Average Rides per Hour");
  }
}

void HeatMap::render(QPainter *painter)
{
  if (this->enabled && this->dataReady) {
    this->useGLPainting(painter);
    this->renderGL();
    if (this->colorBarVisible) {
      this->useQtPainting(painter);
      this->colorBar->paint(painter);
    }
  }
}

void HeatMap::updateData()
{
  this->dataReady = false;
  this->visualDirty = true;
  if (this->enabled)
    this->computeVisualData();
}

inline int coordIndex(float lat, float lon, int width, int height, const QRectF &region)
{
  int latBin = (lat-region.x())/region.width() * (width-1);
  int lonBin = (lon-region.y())/region.height() * (height-1);
  if (latBin<0 || latBin>=width || lonBin<0 || lonBin>=height)
    return -1;;
  return lonBin*width + latBin;
}

void HeatMap::computeVisualData()
{
  if (!this->initialized)
    return;
  int width = this->fbo->size().width();
  int height = this->fbo->size().height();
  this->binCounts.clear();
  this->binCounts.resize(width*height, 0);
  this->maxBinCount = 0;
  
  KdTrip::TripSet::iterator it;
  KdTrip::TripSet *selectedTrips = this->geoWidget->getSelectedTrips();
  Selection::TYPE stype = this->geoWidget->getSelectionType();
  bool usePickup = stype==Selection::START || stype==Selection::START_AND_END;
  bool useDropoff = stype==Selection::END || stype==Selection::START_AND_END;
  for (it=selectedTrips->begin(); it!=selectedTrips->end(); it++) {
    const KdTrip::Trip *trip = *it;
    int index[2] = {-1, -1};
    if (usePickup)
      index[0] = coordIndex(trip->pickup_lat, trip->pickup_long, width, height, this->region);
    if (useDropoff)
      index[1] = coordIndex(trip->dropoff_lat, trip->dropoff_long, width, height, this->region);
    for (int k=0; k<2; k++)
      if (index[k]>=0) {
        this->binCounts[index[k]]++;
        if (this->binCounts[index[k]]>this->maxBinCount)
          this->maxBinCount = this->binCounts[index[k]];
      }
  }

  this->maxValue = this->maxBinCount;
  this->updateColorBar();

  this->dataReady = true;
  this->visualDirty = false;
  this->textureDirty = true;

  emit maxValueUpdated(this->maxValue);
}
  
void HeatMap::initGL()
{
  this->fbo = PQGLFramebufferObject(new QGLFramebufferObject(2048, 2048));
  this->initialized = true;
  if (this->enabled && this->visualDirty)
    this->computeVisualData();
}

void HeatMap::buildHeatMapTexture()
{
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  int width = this->fbo->size().width();
  int height = this->fbo->size().height();
  glViewport(0, 0, width, height);
  glLoadIdentity();
  glOrtho(0, width, 0, height, -99, 99);
  this->fbo->bind();
  glClearColor(1, 1, 1, 1);
  glClear(GL_COLOR_BUFFER_BIT);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
  this->pointTexture.setImage(this->pointImage);

  float hours = this->geoWidget->getSelectionDuration()/3600.0;
  float delta = 9;
  glBegin(GL_QUADS);
  for (int y=0; y<height; y++) {
    for (int x=0; x<width; x++) {
      int c = this->normalized?
        (this->binCounts[y*width+x]*32.0/this->maxValue):
        ceil(this->binCounts[y*width+x]/hours/2.0*8);
      for (int k=0; k<c; k++) {
        glTexCoord2d(0, 0);
        glVertex2f(y-delta, x-delta);
        glTexCoord2d(0, 1);
        glVertex2f(y-delta, x+delta);
        glTexCoord2d(1, 1);
        glVertex2f(y+delta, x+delta);
        glTexCoord2d(1, 0);
        glVertex2f(y+delta, x-delta);
      }
    }
  }
  glEnd();

  QImage img = this->fbo->toImage();
  this->fbo->release();
  glPopMatrix();
  glPopAttrib();

  QImage outImg(img.width(), img.height(), QImage::Format_ARGB32);
  for (int y=0; y<height; y++) {
    QRgb * in = reinterpret_cast<QRgb*>(img.scanLine(y));
    QRgb * out = reinterpret_cast<QRgb*>(outImg.scanLine(y));
    for (int x=0; x<width; x++) {
      float value = qRed(in[x])/255.0;
      if (value>0.2 && value<0.9)
        value = ceil(value/0.1)*0.1;
      QColor color = this->colorScale->getColor(1.0-value);
      color.setAlphaF(value>=0.9?(1-value):0.7);
      out[x] = color.rgba();
    }
  }
  this->textureImage = outImg;
}
 
void HeatMap::renderGL()
{
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);

  if (this->textureDirty) {
    this->buildHeatMapTexture();
    this->heatMapTexture.setImage(this->textureImage);
    this->textureDirty = false;
  }

  QPointF topLeft = this->geoWidget->mapView()->mapFromGeoLocation(this->region.topLeft());
  QPointF bottomRight = this->geoWidget->mapView()->mapFromGeoLocation(this->region.bottomRight());
  
  this->heatMapTexture.bind();
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
  glBegin(GL_QUADS);
  glTexCoord2d(0, 0);
  glVertex2f(topLeft.x(), topLeft.y());
  glTexCoord2d(0, 1);
  glVertex2f(topLeft.x(), bottomRight.y());
  glTexCoord2d(1, 1);
  glVertex2f(bottomRight.x(), bottomRight.y());
  glTexCoord2d(1, 0);
  glVertex2f(bottomRight.x(), topLeft.y());
  glEnd();
  glDisable(GL_BLEND);
  
  glDisable(GL_TEXTURE_2D);
}
