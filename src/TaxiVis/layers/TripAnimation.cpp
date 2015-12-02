#include "TripAnimation.hpp"
#include "ui_TripAnimationConfig.h"
#include "ui_TripAnimationToolBar.h"
#include "QMapView.hpp"
#include "CityMap.hpp"
#include "geographicalviewwidget.h"
#include "global.h"
#include <math.h>
#include <QtCore/QTimer>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QLabel>
#include <QtGui/QLayout>
#include <QtGui/QProgressDialog>
#include <QtGui/QToolBar>
#include <QtGui/QToolButton>
#include <QtOpenGL/QGLShaderProgram>

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

void TripAnimation::setEnabled(bool r)
{
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
  if (this->enabled && this->pathDataReady) {
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
    QDateTime now;
    now.setTime_t(this->globalTime+this->trafficTime);
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

  const QGLContext *context = QGLContext::currentContext();
  if (context) {
    PQGLShaderProgram sh = PQGLShaderProgram(new QGLShaderProgram(context));
    this->shaders.push_back(sh);
    sh->addShaderFromSourceFile(QGLShader::Vertex, ":/Resources/shaders/paths.120.vert");
    sh->addShaderFromSourceFile(QGLShader::Geometry, ":/Resources/shaders/paths.120.geom");
    sh->addShaderFromSourceFile(QGLShader::Fragment, ":/Resources/shaders/paths.120.frag");
    sh->setGeometryInputType(GL_TRIANGLES);
    sh->setGeometryOutputType(GL_TRIANGLE_STRIP);
    sh->link();
  }

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
  PQGLShaderProgram sh = this->shaders[0];
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
  CityMap::Path path;
  CityMap::IntMap nodeId;
  CityMap *city = Global::getInstance()->getMap();
  KdTrip::TripSet::iterator it;
  KdTrip::TripSet *selectedTrips = this->geoWidget->getSelectedTrips();
  uint64_t minPickupTime = (uint64_t)-1;
  uint64_t maxDropoffTime = 0;
  this->maxTrafficTime = 0;
  int cnt = 0;
  this->progress->setLabelText(QString("Computing %1 Shortest Paths").arg(selectedTrips->size()));
  this->progress->setRange(0, selectedTrips->size()-1);
  this->pathVertices.clear();
  this->pathIndices.clear();
  this->pathWeights.clear();
  for (it=selectedTrips->begin(); it!=selectedTrips->end(); it++) {
    this->progress->setValue(cnt++);
    QApplication::processEvents();

    const KdTrip::Trip *trip = *it;
    int src = city->mapToIntersection(CityMap::Location(trip->pickup_lat, trip->pickup_long));
    int dst = city->mapToIntersection(CityMap::Location(trip->dropoff_lat, trip->dropoff_long));
    if (src!=-1 && dst!=-1 && city->findShortestPath(src, dst, path)) {
      float totalTime = 0;
      int wId = this->pathWeights.size();
      for (int i=path.size()-1; i>0; i--) {
        int srcId, dstId;
        CityMap::IntMap::iterator imi = nodeId.find(path[i]);
        if (imi==nodeId.end()) {
          srcId = this->pathVertices.size()/4;
          nodeId[path[i]] = srcId;
          this->pathVertices.push_back(city->getIntersection(path[i]).lon);
          this->pathVertices.push_back(lat2worldY(city->getIntersection(path[i]).lat));
          this->pathVertices.push_back(0);
          this->pathVertices.push_back(0);
        }
        else
          srcId = (*imi).second;
        imi = nodeId.find(path[i-1]);
        if (imi==nodeId.end()) {
          dstId = this->pathVertices.size()/4;
          nodeId[path[i-1]] = dstId;
          this->pathVertices.push_back(city->getIntersection(path[i-1]).lon);
          this->pathVertices.push_back(lat2worldY(city->getIntersection(path[i-1]).lat));
          this->pathVertices.push_back(0);
          this->pathVertices.push_back(0);
        }
        else
          dstId = (*imi).second;

        this->pathIndices.push_back(srcId);
        this->pathIndices.push_back(dstId);
        this->pathIndices.push_back(this->pathWeights.size()/4);

        this->pathWeights.push_back(totalTime);
        totalTime += city->getStreetWeight(CityMap::Street(path[i], path[i-1]));
        this->pathWeights.push_back(totalTime);
        this->pathWeights.push_back(0);
        this->pathWeights.push_back(0);
      }
      totalTime = (trip->dropoff_time-trip->pickup_time)/totalTime;
      for (unsigned i=wId; i<this->pathWeights.size(); i+=4) {
        this->pathWeights[i] = this->pathWeights[i]*totalTime+(trip->pickup_time-this->globalTime);
        this->pathWeights[i+1] = this->pathWeights[i+1]*totalTime+(trip->pickup_time-this->globalTime);
      }
      for (unsigned i=wId; i<this->pathWeights.size(); i+=4) {
        this->pathWeights[i+2] = this->pathWeights[wId];
        this->pathWeights[i+3] = this->pathWeights[this->pathWeights.size()-3];
      }
      if (trip->pickup_time<minPickupTime)
        minPickupTime = trip->pickup_time;
      if (trip->dropoff_time>maxDropoffTime)
        maxDropoffTime = trip->dropoff_time;
    }
  }
  this->pathGeomVertexCount = this->pathVertices.size()/4;
  for (unsigned i=2; i<this->pathIndices.size(); i+=3)
    this->pathIndices[i] += this->pathGeomVertexCount;
  for (unsigned i=0; i<this->pathWeights.size(); i++) {
    this->pathVertices.push_back(this->pathWeights[i]);
    if (i%4<2 && this->maxTrafficTime<this->pathWeights[i])
      this->maxTrafficTime = this->pathWeights[i];
  }
  this->pathBufferDirty = true;
  this->pathDataReady = true;
  this->trafficTime = 0;
  // fprintf(stderr, ">>>> %d %llu\n", this->maxTrafficTime, maxDropoffTime-minPickupTime);

  this->setFrameCount(this->maxTrafficTime+this->trailingPeriod());
}
