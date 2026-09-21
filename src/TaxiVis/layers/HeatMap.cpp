#include <GL/glew.h>
#include "HeatMap.hpp"
#include "QMapTileWidget.hpp"
#include "global.h"
#include "geographicalviewwidget.h"
#include "util/colorbar.h"
#include "util/heatedobjectscale.h"
#include <QVector2D>
#include <array>

// Qt's OpenGL headers undef GLEW macros, so we need to redefine them
#ifndef glBindBuffer
#define glBindBuffer __glewBindBuffer
#endif

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
  if (maxValue==value) return;
  this->maxValue = value;
  updateColorBar();
  if (binCounts) buildHeatMapTexture();
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
    if (binCounts) buildHeatMapTexture();
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
  cancelComputation();
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
  if (!geoWidget->getSelectedTrips() || !geoWidget->hasCurrentSelection()) return;
  textureJob.cancel();
  dataReady=false;
  const auto trips = *geoWidget->getSelectedTrips();
  const auto rect = region;
  const auto size = resolution;
  const auto type = geoWidget->getSelectionType();
  countJob.submit([trips, rect, size, type](const Cancellation &cancel) {
      Counts result; result.bins=std::make_shared<std::vector<int>>(size.width()*size.height(),0);
      size_t n=0;
      for (auto trip : trips) {
          if ((n++ & 1023)==0) cancel.check();
          int indices[2]={-1,-1};
          if (type==Selection::START || type==Selection::START_AND_END)
              indices[0]=coordIndex(trip->pickup_lat,trip->pickup_long,size.width(),size.height(),rect);
          if (type==Selection::END || type==Selection::START_AND_END)
              indices[1]=coordIndex(trip->dropoff_lat,trip->dropoff_long,size.width(),size.height(),rect);
          for (int index : indices) if (index>=0) result.maximum=std::max(result.maximum,++(*result.bins)[index]);
      }
      return result;
  }, [this](Counts result) {
      binCounts=std::move(result.bins); maxBinCount=result.maximum; maxValue=maxBinCount;
      visualDirty=false;
      updateColorBar();
      emit maxValueUpdated(maxValue);
      buildHeatMapTexture();
  });
}
  
void HeatMap::initGL()
{
  this->fbo = PQOpenGLFramebufferObject(new QOpenGLFramebufferObject(2048, 2048));
  this->initialized = true;
  if (this->enabled && this->visualDirty)
    this->computeVisualData();
}

void HeatMap::buildHeatMapTexture()
{
  if (!binCounts) return;
  const auto counts=binCounts;
  const auto size=resolution;
  const auto point=pointImage.scaled(18,18,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
  const bool normalize=normalized;
  const float maximum=maxValue;
  const double hours=std::max(1.0/3600,geoWidget->getSelectionDuration()/3600.0);
  // Snapshot the color scale on the GUI thread. The worker owns only plain images/numbers.
  std::array<QRgb,256> colors;
  for (int i=0; i<256; ++i) {
      float value=i/255.f;
      if (value>0.2 && value<0.9) value=ceil(value/0.1)*0.1;
      auto color=colorScale->getColor(1.0-value);
      color.setAlphaF(value>=0.9 ? 1-value : 0.7);
      colors[i]=color.rgba();
  }
  textureJob.submit([counts,size,point,normalize,maximum,hours,colors](const Cancellation &cancel) {
      const int width=size.width(), height=size.height();
      std::vector<float> transmission(width*height,1.f);
      for (int lon=0; lon<height; ++lon) {
          cancel.check();
          for (int lat=0; lat<width; ++lat) {
              const int count=(*counts)[lon*width+lat];
              if (!count) continue;
              const int repeats=normalize ? (maximum>0 ? int(count*32.0/maximum) : 0) : int(ceil(count/hours*4));
              if (!repeats) continue;
              // Equivalent repeated source-over splats without a loop per ride.
              for (int py=0; py<18; ++py) {
                  const int y=height-1-lat+py-9;
                  if (y<0 || y>=height) continue;
                  for (int px=0; px<18; ++px) {
                      const int x=lon+px-9;
                      if (x<0 || x>=width) continue;
                      const float alpha=qAlpha(point.pixel(px,py))/255.f;
                      transmission[y*width+x]*=std::pow(1-alpha,repeats);
                  }
              }
          }
      }
      QImage image(width,height,QImage::Format_ARGB32);
      for (int y=0; y<height; ++y) {
          cancel.check();
          auto line=reinterpret_cast<QRgb*>(image.scanLine(y));
          for (int x=0; x<width; ++x) line[x]=colors[std::clamp(int(64+191*transmission[y*width+x]),0,255)];
      }
      return image;
  }, [this](QImage image) {
      textureImage=std::move(image); textureDirty=true; dataReady=true;
      geoWidget->repaintContents();
  });
}
 
void HeatMap::renderGL()
{
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);

  if (this->textureDirty) {
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
