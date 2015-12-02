#ifndef Q_MAP_VIEW_HPP
#define Q_MAP_VIEW_HPP

#include <QtCore/QPointF>
#include <QtCore/QTime>
#include <QtGui/QGraphicsSceneMouseEvent>
#include <QtWebKit/QGraphicsWebView>
#include <QtWebKit/QWebView>
#include "RenderingLayer.hpp"

class QMapWidget;
class FpsRenderingLayer;
class RenderingLayerManager;

class QMapView : public QGraphicsWebView
{
  Q_OBJECT
public:
  QMapView(QPointF coords, int level=15, QWidget *parent=0);
  ~QMapView();

  void setMapType(const QString &type);

  QPointF mapFromGeoLocation(QPointF geoCoords) const;
  QPointF mapToGeoLocation(QPointF viewCoords) const;

  int     zoomLevel() const;
  void    setZoomLevel(int level);
  QPointF center() const;
  void    setCenter(QPointF p);
  void    setView(QPointF p, int level);
  
  void    showMap(bool show);
  void    showFps(bool show);
 
  void            addRenderingLayer(RenderingLayer *layer, float depth=-1);
  RenderingLayer* getRenderingLayer(int stackingOrder);
  QList<RenderingLayer*> getRenderingLayers();

signals:
  void viewChanged(QPointF center, int level);
  void doneUpdating();

public slots:
  void loadFinished(bool ok);
  void updateView();
  void syncView();

protected:
  void initView();
  void initGL();
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

  void keyPressEvent(QKeyEvent *event);
  void mousePressEvent(QGraphicsSceneMouseEvent *event);
  void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
  void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
  void resizeEvent(QGraphicsSceneResizeEvent *event);
  void wheelEvent(QGraphicsSceneWheelEvent *event);
  bool sceneEvent(QEvent *event);
  
  int  getMapZoomLevel();
  void setMapZoomLevel(int zoom);
  QPointF getMapCenter();
  void setMapCenter(QPointF p);
  void panMapCenter(QPointF p);
  void invalidateZoomLevel();

private slots:
  void deferredLoadFinished();

private:
  QVariant     js(const QString &cmd);
  bool         glInitialized;  
  bool         showMapEnabled;
  int          lastButtonPressed;
  QPoint       lastPos;
  QPointF      mapCenter;
  int          mapLevel;
  QPointF      lastCenter;
  int          lastLevel;
  int          startedPinchZoomLevel;
  QMapWidget  *mapWidget;

  FpsRenderingLayer *fpsRenderingLayer;
  RenderingLayerManager *layers;
};

#endif
