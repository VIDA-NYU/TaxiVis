#include "QMapView.hpp"
#include "QMapWidget.hpp"
#include <QtCore/QMultiMap>
#include <QtCore/QTimer>
#include <QtGui/QGesture>
#include <QtGui/QMouseEvent>
#include <QtGui/QStyleOptionGraphicsItem>
#include <QtWebKit/QWebFrame>
#include <cmath>

class RenderingLayerManager
{
public:
  typedef QMultiMap<float, RenderingLayer*> RenderingLayerMap;
  typedef RenderingLayerMap::iterator iterator;

  void add(RenderingLayer *layer, float depth=-1.0)
  {
    if (depth<0)
      depth = this->layers.count();
    this->layers.insert(depth, layer);
  }
  
  void remove(RenderingLayer *layer)
  {
    QList<float> k = this->layers.keys(layer);
    for (int i=0; i<k.size(); i++)
      this->layers.remove(k.at(i), layer);
  }

  RenderingLayer *get(float depth)
  {
    return this->layers.value(depth, NULL);
  }
  
  iterator begin()
  {
    return this->layers.begin();
  }
  
  iterator end()
  {
    return this->layers.end();
  }

private:
  RenderingLayerMap layers;
};

class FpsRenderingLayer : public RenderingLayer
{
public:
  FpsRenderingLayer()
      : RenderingLayer(true), visible(false), frameCount(0),
        fpsFont("Times", 16), fpsFontMetrics(this->fpsFont)
  {
  }
  
  void initialize()
  {
    this->fpsTimer.start();
    this->lastFpsTime.start();
  }

  void setVisible(bool b)
  {
    this->visible = b;
  }

  bool isVisible()
  {
    return this->visible;
  }

  void render(QPainter *painter)
  {
    if (this->visible) {
      this->useQtPainting(painter);
      QRect window = painter->window();
      QString fps = QString("%1 fps").arg(this->currentFps, 0, 'f', 1);
      painter->setFont(this->fpsFont);
      painter->setPen(Qt::black);
      painter->drawText(window.right()-this->fpsFontMetrics.width(fps),
                        window.height()-20, fps);
    }
    this->computeFps();
  }
  
  void computeFps()
  {
    int now = this->fpsTimer.elapsed();
    if (this->lastFpsTime.elapsed()>=250) {
      int duration = now-this->frameTime[this->frameCount%5];
      this->currentFps = duration?5000.0/duration:0.0;
      this->lastFpsTime.restart();
    }
    this->frameTime[this->frameCount%5] = now;
    this->frameCount++;
  }
  
private:
  bool         visible;
  int          frameCount;
  int          frameTime[5];
  float        currentFps;
  QTime        fpsTimer;
  QTime        lastFpsTime;
  QFont        fpsFont;
  QFontMetrics fpsFontMetrics;
};

inline double lat2worldY(double lat) {
  return atanh(sin(lat*M_PI/180));
}

inline double lat2y(double lat) {
  return (M_PI-lat2worldY(lat))/M_PI*128;
}

inline double lon2x(double lon) {
  return (lon+180)/360.0*256;
}

inline double y2lat(double y) {
  return atan(sinh(M_PI*(1-y/128)))*180/M_PI;
}

inline double x2lon(double x) {
  return x*360/256-180;
}

class QCustomAgentWebPage: public QWebPage
{
protected:
  QString userAgentForUrl (const QUrl &/*url*/) const
  {
    return QString("Mozilla/5.0 (%Platform%; %Security%; %Subplatform%; %Locale%) AppleWebKit/%WebKitVersion% (KHTML, like Gecko) %AppVersion Firefox/10.0.11");
  }
};

QMapView::QMapView(QPointF coords, int level, QWidget *parent)
    : QGraphicsWebView(), glInitialized(false),
      showMapEnabled(true), lastButtonPressed(-1),
      mapCenter(coords), mapLevel(level),
      lastCenter(coords), lastLevel(level),
      mapWidget(static_cast<QMapWidget*>(parent)),
      fpsRenderingLayer(new FpsRenderingLayer()),
      layers(new RenderingLayerManager())
{    
  this->setAcceptTouchEvents(true);
  this->grabGesture(Qt::PinchGesture);  
  
  this->connect(this, SIGNAL(loadFinished(bool)), this, SLOT(loadFinished(bool)));
  this->settings()->globalSettings()->setAttribute(QWebSettings::LocalStorageEnabled, true);
  this->setPage(new QCustomAgentWebPage());
  this->page()->currentFrame()->addToJavaScriptWindowObject("mapView", this);
  this->setHtml("<!DOCTYPE html>"
                "<html>"
                "  <head>"
                "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">"
                "    <meta charset=\"UTF-8\">"
                "    <style type=\"text/css\">"
                "      html, body, #map_canvas {"
                "        margin: 0;"
                "        padding: 0;"
                "        height: 100%;"
                "      }"
                "    </style>"
                "    <script type=\"text/javascript\" src=\"http://maps.googleapis.com/maps/api/js?sensor=false\"></script>"
                "    <script type=\"text/javascript\">"
                "      var map;"
                "      function initialize() {"
                "        var customType = ["
                "            {"
                "              stylers: [ { saturation: -100 }, { gamma : 2 }]"
                "            }"
                "        ];"
                "        var initialOptions = {"
                "          disableDefaultUI: true,"
                "          disableDoubleClickZoom: true,"
                "          mapTypeId: google.maps.MapTypeId.TERRAIN"
                "        };"
                "        map = new google.maps.Map(document.getElementById('map_canvas'), initialOptions);"
                "        map.mapTypes.set(\"Custom\", new google.maps.StyledMapType(customType, { name: \"Custom\"}));"
                "        map.setMapTypeId(\"Custom\");"
                "        map.setTilt(0);"
                "        google.maps.event.addListener(map, 'idle', window.mapView.syncView);"
                "      }"
                "      google.maps.event.addDomListener(window, 'load', initialize);"
                "    </script>"
                "  </head>"
                "  <body>"
                "    <div id=\"map_canvas\"></div>"
                "  </body>"
                "</html>");
}

QMapView::~QMapView()
{
  delete this->fpsRenderingLayer;
  delete this->layers;
}

void QMapView::syncView()
{
  if (this->mapCenter!=this->getMapCenter())
    this->setMapCenter(this->mapCenter);
  emit doneUpdating();
}

int QMapView::zoomLevel() const
{
  return this->mapLevel;
}

void QMapView::setZoomLevel(int level)
{
  this->mapLevel = level;
  this->updateView();
  this->invalidateZoomLevel();
}

QPointF QMapView::center() const
{
  return this->mapCenter;
}

void QMapView::setCenter(QPointF p)
{
  this->mapCenter = p;
  this->updateView();
}

void QMapView::setView(QPointF p, int level)
{
  this->mapCenter = p;
  this->mapLevel = level;
  this->updateView();
}

void QMapView::showMap(bool show)
{
  this->showMapEnabled = show;
  this->update();
}

void QMapView::showFps(bool show)
{
  this->fpsRenderingLayer->setVisible(show);
  this->update();
}

void QMapView::loadFinished(bool /*ok*/)
{
  QTimer::singleShot(0, this, SLOT(deferredLoadFinished()));
  this->updateView();
}

void QMapView::deferredLoadFinished()
{
  if (this->mapWidget)
    this->mapWidget->loadFinished();
}

QVariant QMapView::js(const QString &cmd)
{
  return this->page()->currentFrame()->evaluateJavaScript(cmd);
}

void QMapView::setMapZoomLevel(int zoom)
{
  this->js(QString("map.setZoom(%1)").arg(zoom));
}

int QMapView::getMapZoomLevel()
{
  QVariant res = this->js("map.getZoom()");
  return res.toInt();
}

QPointF QMapView::getMapCenter()
{
  QMap<QString, QVariant> coords = this->js("map.getCenter()").toMap();
  return QPointF(coords["hb"].toDouble(), coords["ib"].toDouble());
}

void QMapView::setMapType(const QString &type)
{
  if (type=="Custom")
    this->js(QString("map.setMapTypeId(\"Custom\");"));
  else
    this->js(QString("map.setMapTypeId(google.maps.MapTypeId.%1);").arg(type));
}

void QMapView::setMapCenter(QPointF p)
{
  this->js(QString("map.setCenter(new google.maps.LatLng(%1,%2,true));").arg(p.x(),0,'g',15).arg(p.y(),0,'g',15));
}

void QMapView::panMapCenter(QPointF p)
{
  this->js(QString("map.panTo(new google.maps.LatLng(%1,%2,true));").arg(p.x(),0,'g',15).arg(p.y(),0,'g',15));
}

void QMapView::updateView()
{
  this->js(QString("map.setCenter(new google.maps.LatLng(%1,%2,true));"
                   "map.setZoom(%3);")
           .arg(this->mapCenter.x(),0,'g',15)
           .arg(this->mapCenter.y(),0,'g',15)
           .arg(this->mapLevel)
           );
  if (this->mapCenter!=this->lastCenter || this->mapLevel!=this->lastLevel) {
    this->lastCenter = this->mapCenter;
    this->lastLevel = this->mapLevel;
    emit viewChanged(this->lastCenter, this->lastLevel);
  }
}

void QMapView::keyPressEvent(QKeyEvent *event)
{
  int key = event->key();
  double dist = exp2((double)(6-this->mapLevel));
  double dlat = 0, dlng=0;
  event->accept();
  switch (key) {
  case Qt::Key_Left:
  case Qt::Key_Right:
  case Qt::Key_Up:
  case Qt::Key_Down:
    if (key==Qt::Key_Left) dlng = -dist;
    if (key==Qt::Key_Right) dlng = dist;
    if (key==Qt::Key_Up) dlat = dist;
    if (key==Qt::Key_Down) dlat = -dist;

    this->mapCenter.setX(this->mapCenter.x() + dlat);
    this->mapCenter.setY(this->mapCenter.y() + dlng);
    this->updateView();
    break;

  case Qt::Key_Minus:
  case Qt::Key_Equal:
  case Qt::Key_Plus:
    if (key==Qt::Key_Minus)
      this->setZoomLevel(this->mapLevel-1);
    else
      this->setZoomLevel(this->mapLevel+1);
    break;

  case Qt::Key_H:
    this->setMapType("HYBRID");
    break;
  case Qt::Key_R:
    this->setMapType("ROADMAP");
    break;
  case Qt::Key_S:
    this->setMapType("SATELLITE");
    break;
  case Qt::Key_T:
    this->setMapType("TERRAIN");
    break;
  case Qt::Key_G:
    this->setMapType("Custom");
    break;

  case Qt::Key_M:
    this->showMap(!this->showMapEnabled);
    break;

  case Qt::Key_F:
    this->showFps(!this->fpsRenderingLayer->isVisible());
    break;

  default:
    event->ignore();
    break;
  }
}

void QMapView::invalidateZoomLevel()
{
  int updatedLevel = this->getMapZoomLevel();
  if (updatedLevel<this->mapLevel) {
    this->mapLevel = updatedLevel;
    this->updateView();
  }  
}

void QMapView::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
  this->lastButtonPressed = event->button();
  this->lastPos = event->screenPos();
}

QPointF QMapView::mapFromGeoLocation(QPointF geoCoords) const
{
  double unit = exp2((double)this->mapLevel);
  return QPointF((lon2x(geoCoords.y())-lon2x(this->mapCenter.y()))*unit+this->size().width()*0.5,
                 (lat2y(geoCoords.x())-lat2y(this->mapCenter.x()))*unit+this->size().height()*0.5);
}

QPointF QMapView::mapToGeoLocation(QPointF viewCoords) const
{
  double unit = exp2((double)this->mapLevel);
  return QPointF(y2lat((viewCoords.y()-this->size().height()*0.5)/unit+lat2y(this->mapCenter.x())),
                 x2lon((viewCoords.x()-this->size().width()*0.5)/unit+lon2x(this->mapCenter.y())));
}

void QMapView::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
  if (this->lastButtonPressed!=-1) {
    double unit = exp2((double)this->mapLevel);
    QPoint dist = event->screenPos()-this->lastPos;
    double x = lon2x(this->mapCenter.y()) - dist.x()/unit;
    double y = lat2y(this->mapCenter.x()) - dist.y()/unit;
    this->mapCenter.setX(y2lat(y));
    this->mapCenter.setY(x2lon(x));
    this->updateView();
    this->lastPos = event->screenPos();
  }
}

void QMapView::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
  this->lastButtonPressed = -1;
}

void QMapView::resizeEvent(QGraphicsSceneResizeEvent *event)
{
  QGraphicsWebView::resizeEvent(event);
  this->updateView();
}

void QMapView::wheelEvent(QGraphicsSceneWheelEvent *event)
{
  if (event->delta()>0)
    this->setZoomLevel(this->mapLevel+1);
  if (event->delta()<0)
    this->setZoomLevel(this->mapLevel-1);
}

void QMapView::initGL() {
  if (this->glInitialized) return;
  RenderingLayerManager::iterator it;
  for (it=this->layers->begin(); it!=this->layers->end(); it++)
    it.value()->initGL();
  this->fpsRenderingLayer->initGL();
  if (this->mapWidget)
    this->mapWidget->initGL();
  this->glInitialized = true;
}

bool QMapView::sceneEvent(QEvent *event)
{
  QPinchGesture *pinch;
  
  switch (event->type()) {
  case QEvent::TouchBegin:
  case QEvent::TouchUpdate:
  case QEvent::TouchEnd:
    return true;

  case QEvent::Gesture:
    pinch = static_cast<QPinchGesture*>(static_cast<QGestureEvent*>(event)->gesture(Qt::PinchGesture));
    switch (pinch->state()) {
    case Qt::GestureStarted:
      this->startedPinchZoomLevel = this->mapLevel;
      break;
    case Qt::GestureUpdated:
      this->setZoomLevel(this->startedPinchZoomLevel + (int)(log(pinch->scaleFactor())/log(1.5)));
      break;
    default:
      break;
    }
    return true;      

  default:
    break;
  }

  return QGraphicsWebView::sceneEvent(event);
}

void QMapView::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
  this->initGL();
  
  if (this->showMapEnabled)
    QGraphicsWebView::paint(painter, option, widget);
  
  bool glMode = false;
  RenderingLayerManager::iterator it;
  for (it=this->layers->begin(); it!=this->layers->end(); it++)
    glMode = it.value()->paint(painter, glMode);
  
  if (this->mapWidget)
    this->mapWidget->paintOverlay(painter);
  
  this->fpsRenderingLayer->paint(painter, glMode);
}

void QMapView::addRenderingLayer(RenderingLayer *layer, float depth)
{
  this->layers->add(layer, depth);
}

RenderingLayer* QMapView::getRenderingLayer(int stackingOrder)
{
  return this->layers->get(stackingOrder);
}

QList<RenderingLayer*> QMapView::getRenderingLayers()
{
  QList<RenderingLayer*> results;
  RenderingLayerManager::iterator it;
  for (it=this->layers->begin(); it!=this->layers->end(); it++)
    results.append(it.value());
  return results;
}
