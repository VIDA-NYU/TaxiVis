#include <GL/glew.h>
#include "TripAnimation.hpp"
#include "ui_TripAnimationConfig.h"
#include "ui_TripAnimationToolBar.h"
#include "QMapTileWidget.hpp"
#include "CityMap.hpp"
#include "geographicalviewwidget.h"
#include "global.h"
#include <math.h>
#include <QTimer>
#include <QAction>
#include <QApplication>
#include <QLabel>
#include <QLayout>
#include <QProgressDialog>
#include <QToolBar>
#include <QToolButton>
#include <QOpenGLShaderProgram>

// Qt's OpenGL headers undef GLEW macros, so we need to redefine them
#ifndef glBindBuffer
#define glBindBuffer __glewBindBuffer
#endif

TripAnimation::TripAnimation(GeographicalViewWidget *gw) :
    RenderingLayer(false),
    uiConfig(new Ui::TripAnimationConfig),
    uiToolBar(new Ui::TripAnimationToolBar),
    playing(false), pathDataReady(false), pathBufferDirty(false),
    trafficTime(0), maxTrafficTime(0), globalTime(0),
    font("Arial", 32, QFont::Bold)
{
  this->geoWidget = gw;
  this->progress = new QProgressDialog(QString(), QString(), 0, 0, this->geoWidget);
  // QProgressDialog starts a timer in its constructor and pops up on its own
  // after minimumDuration (4s). reset() stops that timer so the dialog only
  // appears once buildAnimPath() starts reporting progress via setValue().
  this->progress->reset();
  
  this->configWidget = new QWidget();
  this->uiConfig->setupUi(this->configWidget);
  
  this->toolBarWidget = new QWidget();
  this->uiToolBar->setupUi(this->toolBarWidget);
  this->uiToolBar->frameSlider->setVisible(false);

  this->connect(this->uiToolBar->configButton, SIGNAL(toggled(bool)), this, SLOT(showConfig(bool)));
  this->connect(this->uiToolBar->playButton, SIGNAL(toggled(bool)), this, SLOT(setPlaying(bool)));
  this->connect(this->uiToolBar->resetButton, SIGNAL(clicked()), this, SLOT(reset()));

  this->connect(this->uiToolBar->frameButton, SIGNAL(toggled(bool)), this->uiToolBar->frameSlider, SLOT(setVisible(bool)));
  this->connect(this->uiToolBar->frameSlider, SIGNAL(valueChanged(int)), this, SLOT(setCurrentFrame(int)));
  this->connect(this->uiToolBar->frameSlider, SIGNAL(sliderPressed()), this, SLOT(stop()));

  this->connect(this->uiConfig->glyphSizeSlider, SIGNAL(valueChanged(int)), this, SLOT(glyphSizeSliderChanged(int)));
  this->connect(this->uiConfig->speedSlider, SIGNAL(valueChanged(int)), this, SLOT(setPlaySpeed(int)));
  this->connect(this->uiConfig->trailingPeriodSlider, SIGNAL(valueChanged(int)), this, SLOT(trailingPeriodSliderChanged(int)));

  this->setGlyphSize(1);
  this->setTrailingPeriod(30);
  this->setPlaySpeed(3);
}

TripAnimation::~TripAnimation()
{
  delete this->progress;
  delete this->toolBarWidget;
  delete this->uiToolBar;
}

bool TripAnimation::isSupported() const
{
  // The animated paths need the geometry shader loaded in initGL(). It is
  // not loaded on macOS (no geometry shaders in the compatibility profile
  // the rest of the renderer requires), so the layer cannot be enabled.
  return !this->shaders.empty();
}

void TripAnimation::setEnabled(bool r)
{
  if (r && !this->isSupported())
    r = false;
  if (this->enabled!=r) {
    this->enabled = r;

    if (this->enabled) {
      if (!this->pathDataReady)
        this->buildAnimPath();
      this->showConfig(this->uiToolBar->configButton->isChecked());
    }
    else {
      this->stop();
      this->showConfig(false);
      this->progress->hide();
    }

    this->toolBar->setVisible(this->enabled);
  }
}

bool TripAnimation::isPlaying()
{
  return this->playing;
}

void TripAnimation::togglePlaying()
{
  if (this->enabled)
    this->setPlaying(!this->playing);
}

void TripAnimation::setPlaying(bool r)
{
  if (this->playing!=r) {
    this->playing = r;
    if (this->playing) {
      this->geoWidget->setViewportUpdateMode(QGraphicsView::NoViewportUpdate);
      this->startNextFrame();
    }
    else {
      this->geoWidget->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    }

    this->uiToolBar->playButton->setChecked(this->playing);
  }
}

void TripAnimation::play()
{
  this->setEnabled(true);
  this->setPlaying(true);
}

void TripAnimation::stop()
{
  this->setPlaying(false);
}

void TripAnimation::reset()
{
  this->setCurrentFrame(0);
}

void TripAnimation::showConfig(bool r)
{
  this->config->setVisible(r);
}

qreal TripAnimation::glyphSize()
{
  return this->glyphArea.y();
}

void TripAnimation::setGlyphSize(qreal size)
{
  this->glyphArea.setY(size);
  this->uiConfig->glyphSizeSlider->setValue(size/0.1);
}

qreal TripAnimation::trailingPeriod()
{
  return this->glyphArea.x();
}

void TripAnimation::setTrailingPeriod(qreal seconds)
{
  this->glyphArea.setX(seconds);
  this->uiConfig->trailingPeriodSlider->setValue(seconds);
  this->setFrameCount(this->maxTrafficTime+seconds);
}

int TripAnimation::playSpeed()
{
  return this->frameInterval;
}

void TripAnimation::setPlaySpeed(int speed)
{
  this->frameInterval = speed;
  this->uiConfig->speedSlider->setValue(speed);
}

int TripAnimation::frameCount()
{
  return this->uiToolBar->frameSlider->maximum();
}

void TripAnimation::setFrameCount(int cnt)
{
  this->uiToolBar->frameSlider->setRange(0, cnt);
}

int TripAnimation::currentFrame()
{
  return this->trafficTime;
}

void TripAnimation::setCurrentFrame(int frame)
{
  if (frame!=this->trafficTime) {
    this->trafficTime = std::min(frame, this->frameCount());
    this->uiToolBar->frameSlider->blockSignals(true);
    this->uiToolBar->frameSlider->setValue(frame);
    this->geoWidget->repaintContents();
    this->uiToolBar->frameSlider->blockSignals(false);
  }
}

void TripAnimation::render(QPainter *painter)
{
  this->useGLPainting(painter);
  this->renderGL();
  this->useQtPainting(painter);
  this->renderQt(painter);
}

void TripAnimation::renderGL()
{
  if (this->enabled && this->pathDataReady && this->isSupported()) {
    if (this->pathBufferDirty) {
      this->updatePathBuffers();
      this->pathBufferDirty = false;
    }
    this->renderPaths();
  }
}  

void TripAnimation::renderQt(QPainter *painter)
{
  if (this->enabled && this->pathDataReady) {
    QDateTime now = QDateTime::fromSecsSinceEpoch(this->globalTime+this->trafficTime);
    QString txt = now.toString("MM/dd/yy hh:mm:ss");
    QPainterPath path;
    path.addText(10, painter->window().height()-35, this->font, txt);
    painter->setPen(Qt::black);
    painter->setBrush(Qt::white);
    painter->drawPath(path);
  }
}

void TripAnimation::startNextFrame()
{
  if (!this->enabled) return;
  this->setCurrentFrame(this->trafficTime+this->frameInterval);
  if (this->playing) {
    if (this->trafficTime<this->frameCount())
      QTimer::singleShot(15, this, SLOT(startNextFrame()));
    else
      this->setPlaying(false);
  }
}

void TripAnimation::updateData()
{
  pathJob.cancel();
  this->pathDataReady = false;
  QDateTime startTime = this->geoWidget->getSelectedStartTime();
  QDate d = startTime.date();
  QTime t = startTime.time();
  this->globalTime = KdTrip::Query::createTime(d.year(), d.month(), d.day(), t.hour(), t.minute(), t.second());
  if (this->enabled)
    this->buildAnimPath();  
}

void TripAnimation::initGL()
{
  glewInit();

  this->bufPath[0].generate();
  this->bufPath[1].generate();

  // Geometry shaders are not supported on macOS in compatibility profile
  // TripAnimation (animated paths) is disabled on this platform
  // Users can still use TripLocation layer to visualize pickup/dropoff points

  this->toolBar = this->geoWidget->scene()->addWidget(this->toolBarWidget);
  this->toolBar->setVisible(this->enabled);
  
  this->config = this->geoWidget->scene()->addWidget(this->configWidget);
  this->config->setPos(10, this->toolBar->geometry().height());
  this->config->setVisible(this->uiToolBar->configButton->isChecked());
}

void TripAnimation::glyphSizeSliderChanged(int value)
{
  this->setGlyphSize(value*0.1);
}

void TripAnimation::trailingPeriodSliderChanged(int value)
{
  this->setTrailingPeriod((int)value);
}

void TripAnimation::updatePathBuffers()
{
  this->bufPath[0].setData(GL_ARRAY_BUFFER, this->pathVertices.size()*sizeof(float), &this->pathVertices[0], GL_DYNAMIC_DRAW);
  this->bufPath[1].setData(GL_ELEMENT_ARRAY_BUFFER, this->pathIndices.size()*sizeof(GLuint), &this->pathIndices[0], GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

inline double lat2worldY(double lat) {
    return atanh(sin(lat*M_PI/180));
}

void TripAnimation::renderPaths()
{
  QPointF center = this->geoWidget->mapView()->center();
  int zoomLevel = this->geoWidget->mapView()->zoomLevel();
  QSizeF size = this->geoWidget->mapView()->size();
  PQOpenGLShaderProgram sh = this->shaders[0];
  sh->bind();
  sh->setUniformValue("nVert", this->pathGeomVertexCount);
  sh->setUniformValue("zoom", zoomLevel);
  sh->setUniformValue("center", QVector2D(center.y(), lat2worldY(center.x())));
  sh->setUniformValue("scale", QVector2D(180*size.width(), M_PI*size.height()));
  sh->setUniformValue("time", (float)this->trafficTime);
  sh->setUniformValue("glyphArea", this->glyphArea);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glBindBuffer(GL_ARRAY_BUFFER, this->bufPath[0].buffer);
  sh->enableAttributeArray("pos");
  sh->setAttributeArray("pos", GL_FLOAT, 0, 4, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->bufPath[1].buffer);
  glDrawElements(GL_TRIANGLES, this->pathIndices.size(), GL_UNSIGNED_INT, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glDisable(GL_BLEND);
  
  sh->release();
}

void TripAnimation::buildAnimPath()
{
  if (!geoWidget->getSelectedTrips() || !geoWidget->hasCurrentSelection()) return;
  const auto trips=*geoWidget->getSelectedTrips();
  const auto epoch=globalTime;
  const CityMap *city=Global::getInstance()->getMap();
  pathJob.submit([trips, epoch, city](const Cancellation &cancel) {
  PathData result;
  CityMap::Path path;
  CityMap::IntMap nodeId;
  KdTrip::TripSet::iterator it;
  uint64_t minPickupTime = (uint64_t)-1;
  uint64_t maxDropoffTime = 0;
  result.maxTrafficTime = 0;
  int cnt = 0;
  result.pathVertices.clear();
  result.pathIndices.clear();
  result.pathWeights.clear();
  for (it=trips.begin(); it!=trips.end(); it++) {
    cancel.check();

    const KdTrip::Trip *trip = *it;
    int src = city->mapToIntersection(CityMap::Location(trip->pickup_lat, trip->pickup_long));
    int dst = city->mapToIntersection(CityMap::Location(trip->dropoff_lat, trip->dropoff_long));
    if (src!=-1 && dst!=-1 && city->computeShortestPath(src, dst, path)) {
      float totalTime = 0;
      int wId = result.pathWeights.size();
      for (int i=path.size()-1; i>0; i--) {
        int srcId, dstId;
        CityMap::IntMap::iterator imi = nodeId.find(path[i]);
        if (imi==nodeId.end()) {
          srcId = result.pathVertices.size()/4;
          nodeId[path[i]] = srcId;
          result.pathVertices.push_back(city->getIntersection(path[i]).lon);
          result.pathVertices.push_back(lat2worldY(city->getIntersection(path[i]).lat));
          result.pathVertices.push_back(0);
          result.pathVertices.push_back(0);
        }
        else
          srcId = (*imi).second;
        imi = nodeId.find(path[i-1]);
        if (imi==nodeId.end()) {
          dstId = result.pathVertices.size()/4;
          nodeId[path[i-1]] = dstId;
          result.pathVertices.push_back(city->getIntersection(path[i-1]).lon);
          result.pathVertices.push_back(lat2worldY(city->getIntersection(path[i-1]).lat));
          result.pathVertices.push_back(0);
          result.pathVertices.push_back(0);
        }
        else
          dstId = (*imi).second;

        result.pathIndices.push_back(srcId);
        result.pathIndices.push_back(dstId);
        result.pathIndices.push_back(result.pathWeights.size()/4);

        result.pathWeights.push_back(totalTime);
        totalTime += city->getStreetWeight(CityMap::Street(path[i], path[i-1]));
        result.pathWeights.push_back(totalTime);
        result.pathWeights.push_back(0);
        result.pathWeights.push_back(0);
      }
      totalTime = (trip->dropoff_time-trip->pickup_time)/totalTime;
      for (unsigned i=wId; i<result.pathWeights.size(); i+=4) {
        result.pathWeights[i] = result.pathWeights[i]*totalTime+(trip->pickup_time-epoch);
        result.pathWeights[i+1] = result.pathWeights[i+1]*totalTime+(trip->pickup_time-epoch);
      }
      for (unsigned i=wId; i<result.pathWeights.size(); i+=4) {
        result.pathWeights[i+2] = result.pathWeights[wId];
        result.pathWeights[i+3] = result.pathWeights[result.pathWeights.size()-3];
      }
      if (trip->pickup_time<minPickupTime)
        minPickupTime = trip->pickup_time;
      if (trip->dropoff_time>maxDropoffTime)
        maxDropoffTime = trip->dropoff_time;
    }
  }
  result.pathGeomVertexCount = result.pathVertices.size()/4;
  for (unsigned i=2; i<result.pathIndices.size(); i+=3)
    result.pathIndices[i] += result.pathGeomVertexCount;
  for (unsigned i=0; i<result.pathWeights.size(); i++) {
    result.pathVertices.push_back(result.pathWeights[i]);
    if (i%4<2 && result.maxTrafficTime<result.pathWeights[i])
      result.maxTrafficTime = result.pathWeights[i];
  }
  return result;
  }, [this](PathData result) {
      pathVertices=std::move(result.pathVertices); pathWeights=std::move(result.pathWeights);
      pathIndices=std::move(result.pathIndices); pathGeomVertexCount=result.pathGeomVertexCount;
      maxTrafficTime=result.maxTrafficTime;
      pathBufferDirty=true; pathDataReady=true; trafficTime=0;
      setFrameCount(maxTrafficTime+trailingPeriod());
      geoWidget->repaintContents();
  });
}
