#ifndef Q_MAP_TILE_WIDGET_HPP
#define Q_MAP_TILE_WIDGET_HPP

#include <QGraphicsWidget>
#include <QPointF>
#include <QTime>
#include <QElapsedTimer>
#include <QGraphicsSceneMouseEvent>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPixmap>
#include <QCache>
#include "RenderingLayer.hpp"

class QMapWidget;
class FpsRenderingLayer;
class RenderingLayerManager;

// Represents a single map tile
struct MapTile {
  int x, y, z;  // Tile coordinates
  QPixmap pixmap;
  bool loading;

  MapTile() : x(0), y(0), z(0), loading(false) {}
  MapTile(int x, int y, int z) : x(x), y(y), z(z), loading(false) {}

  QString key() const {
    return QString("%1_%2_%3").arg(z).arg(x).arg(y);
  }
};

class QMapTileWidget : public QGraphicsWidget
{
  Q_OBJECT
public:
  QMapTileWidget(QPointF coords, int level=15, QWidget *parent=0);
  ~QMapTileWidget();

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
  void updateView();

protected:
  void initGL();
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

  void keyPressEvent(QKeyEvent *event);
  void mousePressEvent(QGraphicsSceneMouseEvent *event);
  void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
  void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
  void resizeEvent(QGraphicsSceneResizeEvent *event);
  void wheelEvent(QGraphicsSceneWheelEvent *event);
  bool sceneEvent(QEvent *event);

private:
  bool loadTile(int x, int y, int z);
  void loadVisibleTiles();
  QPointF latLonToTilePixel(double lat, double lon, int zoom) const;
  QString getTileCachePath() const;
  QString getTileCacheFilePath(int x, int y, int z) const;
  bool loadTileFromDisk(int x, int y, int z);
  void saveTileToDisk(int x, int y, int z, const QPixmap &pixmap);

  bool         glInitialized;
  bool         showMapEnabled;
  int          lastButtonPressed;
  QPoint       lastPos;
  QPointF      mapCenter;  // lat, lon
  int          mapLevel;
  QPointF      initialCenter;  // Initial center for reset
  int          initialLevel;   // Initial zoom level for reset
  QPointF      lastCenter;
  int          lastLevel;
  int          startedPinchZoomLevel;
  QMapWidget  *mapWidget;

  QString      tileServerUrl;
  QString      tileCachePath;
  QNetworkAccessManager *networkManager;
  QCache<QString, MapTile> tileCache;

  FpsRenderingLayer *fpsRenderingLayer;
  RenderingLayerManager *layers;
};

#endif
