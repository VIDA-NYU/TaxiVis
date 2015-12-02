#ifndef Q_MAP_WIDGET_HPP
#define Q_MAP_WIDGET_HPP

#include <GL/glew.h>
#include <QtGui/QGraphicsView>
#include <QtGui/QSlider>
#include <QtGui/QGraphicsProxyWidget>
#include <QtOpenGL/QGLWidget>
#include <QtWebKit/QGraphicsWebView>
#include <vector>

class QMapView;
class RenderingLayer;

class QMapWidget : public QGraphicsView
{
  Q_OBJECT
public:
  QMapWidget(QWidget *parent=0);
  QMapWidget(QPointF coords, int level=15, QWidget *parent=0);
  virtual ~QMapWidget() {}

  QMapView *mapView() { return this->mView; }
  void repaintContents();

  virtual void loadFinished() {}
  virtual void initGL() {}
  virtual void paintOverlay(QPainter */*painter*/) {}

protected:
  void initWidget(QPointF coords, int level);
  void resizeEvent(QResizeEvent *event);
  void closeEvent(QCloseEvent *event);
  
private:
  QMapView *mView;
};

#endif
