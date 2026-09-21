#include "QMapTileWidget.hpp"
#include "QMapWidget.hpp"
#include <QMultiMap>
#include <QTimer>
#include <QGesture>
#include <QMouseEvent>
#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <cmath>

// RenderingLayerManager implementation (shared with original QMapView)
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

// FPS rendering layer (shared with original QMapView)
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
      painter->drawText(window.right()-this->fpsFontMetrics.horizontalAdvance(fps),
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
  QElapsedTimer fpsTimer;
  QElapsedTimer lastFpsTime;
  QFont        fpsFont;
  QFontMetrics fpsFontMetrics;
};

// Coordinate transformation functions (from original QMapView)
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

// Tile coordinate functions
inline int long2tilex(double lon, int z) {
  return (int)(floor((lon + 180.0) / 360.0 * (1 << z)));
}

inline int lat2tiley(double lat, int z) {
  double latrad = lat * M_PI/180.0;
  return (int)(floor((1.0 - asinh(tan(latrad)) / M_PI) / 2.0 * (1 << z)));
}

inline double tilex2long(int x, int z) {
  return x / (double)(1 << z) * 360.0 - 180;
}

inline double tiley2lat(int y, int z) {
  double n = M_PI - 2.0 * M_PI * y / (double)(1 << z);
  return 180.0 / M_PI * atan(0.5 * (exp(n) - exp(-n)));
}

QMapTileWidget::QMapTileWidget(QPointF coords, int level, QWidget *parent)
    : QGraphicsWidget(), glInitialized(false),
      showMapEnabled(true), lastButtonPressed(-1),
      mapCenter(coords), mapLevel(level),
      initialCenter(coords), initialLevel(level),
      lastCenter(coords), lastLevel(level),
      mapWidget(static_cast<QMapWidget*>(parent)),
      fpsRenderingLayer(new FpsRenderingLayer()),
      layers(new RenderingLayerManager())
{
  this->setAcceptTouchEvents(true);
  this->grabGesture(Qt::PinchGesture);

  // Set cache size to hold ~1000 tiles (each tile costs 1)
  // This prevents tiles from being evicted during normal panning
  this->tileCache.setMaxCost(1000);

  // Use OpenStreetMap tile server
  // Note: For production, consider using a different tile server or self-hosting
  this->tileServerUrl = "https://tile.openstreetmap.org/{z}/{x}/{y}.png";

  // Initialize network manager for tile downloads
  this->networkManager = new QNetworkAccessManager(this);
  connect(this->networkManager, &QNetworkAccessManager::finished,
          [this](QNetworkReply *reply) {
    QString key = reply->request().attribute(QNetworkRequest::User).toString();

    if (reply->error() == QNetworkReply::NoError) {
      QByteArray data = reply->readAll();
      QPixmap pixmap;
      pixmap.loadFromData(data);

      // Convert to grayscale for better data visualization
      if (!pixmap.isNull()) {
        QImage img = pixmap.toImage().convertToFormat(QImage::Format_ARGB32);
        for (int y = 0; y < img.height(); y++) {
          for (int x = 0; x < img.width(); x++) {
            QRgb pixel = img.pixel(x, y);
            int gray = qGray(pixel);
            img.setPixel(x, y, qRgba(gray, gray, gray, qAlpha(pixel)));
          }
        }
        pixmap = QPixmap::fromImage(img);
      }

      MapTile *tile = this->tileCache.object(key);
      if (tile && !pixmap.isNull()) {
        tile->pixmap = pixmap;
        tile->loading = false;

        // Save tile to disk cache for future use
        QStringList parts = key.split('_');
        if (parts.size() == 3) {
          int z = parts[0].toInt();
          int x = parts[1].toInt();
          int y = parts[2].toInt();
          this->saveTileToDisk(x, y, z, pixmap);
        }

        this->update();
      }
    }

    reply->deleteLater();
  });

  // Set cache size (100 tiles * ~50KB each = ~5MB)
  this->tileCache.setMaxCost(100);

  // Initialize disk cache directory
  this->tileCachePath = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/tiles";
  QDir().mkpath(this->tileCachePath);

  // Set flags for proper rendering
  this->setFlag(QGraphicsItem::ItemUsesExtendedStyleOption, true);

  // Load initial tiles
  QTimer::singleShot(100, this, SLOT(updateView()));
}

QMapTileWidget::~QMapTileWidget()
{
  delete this->fpsRenderingLayer;
  delete this->layers;
}

int QMapTileWidget::zoomLevel() const
{
  return this->mapLevel;
}

void QMapTileWidget::setZoomLevel(int level)
{
  // Clamp zoom level to valid range
  level = qBound(0, level, 19);
  this->mapLevel = level;
  this->updateView();
}

QPointF QMapTileWidget::center() const
{
  return this->mapCenter;
}

void QMapTileWidget::setCenter(QPointF p)
{
  this->mapCenter = p;
  this->updateView();
}

void QMapTileWidget::setView(QPointF p, int level)
{
  this->mapCenter = p;
  this->mapLevel = qBound(0, level, 19);
  this->updateView();
}

void QMapTileWidget::showMap(bool show)
{
  this->showMapEnabled = show;
  this->update();
}

void QMapTileWidget::showFps(bool show)
{
  this->fpsRenderingLayer->setVisible(show);
  this->update();
}

void QMapTileWidget::setMapType(const QString &type)
{
  // For now, we only support OpenStreetMap tiles
  // Could be extended to support different tile servers
  Q_UNUSED(type);
}

void QMapTileWidget::updateView()
{
  loadVisibleTiles();

  if (this->mapCenter != this->lastCenter || this->mapLevel != this->lastLevel) {
    this->lastCenter = this->mapCenter;
    this->lastLevel = this->mapLevel;
    emit viewChanged(this->lastCenter, this->lastLevel);
  }

  this->update();
  emit doneUpdating();
}

QPointF QMapTileWidget::latLonToTilePixel(double lat, double lon, int zoom) const
{
  double n = pow(2.0, zoom);
  double xtile = (lon + 180.0) / 360.0 * n;
  double latrad = lat * M_PI / 180.0;
  double ytile = (1.0 - asinh(tan(latrad)) / M_PI) / 2.0 * n;
  return QPointF(xtile * 256, ytile * 256);
}

void QMapTileWidget::loadVisibleTiles()
{
  if (this->size().width() <= 0 || this->size().height() <= 0)
    return;

  // Calculate center tile
  int centerTileX = long2tilex(this->mapCenter.y(), this->mapLevel);
  int centerTileY = lat2tiley(this->mapCenter.x(), this->mapLevel);

  // Calculate how many tiles we need in each direction
  int tilesX = (int)ceil(this->size().width() / 256.0) + 1;
  int tilesY = (int)ceil(this->size().height() / 256.0) + 1;

  int maxTile = (1 << this->mapLevel) - 1;

  bool anyTilesLoaded = false;

  // Load tiles in a grid around the center
  for (int dy = -tilesY; dy <= tilesY; dy++) {
    for (int dx = -tilesX; dx <= tilesX; dx++) {
      int tx = centerTileX + dx;
      int ty = centerTileY + dy;

      // Wrap x coordinate (longitude wraps around)
      if (tx < 0) tx += (1 << this->mapLevel);
      if (tx > maxTile) tx -= (1 << this->mapLevel);

      // Clamp y coordinate (latitude doesn't wrap)
      if (ty < 0 || ty > maxTile)
        continue;

      if (loadTile(tx, ty, this->mapLevel))
        anyTilesLoaded = true;
    }
  }

  // Only trigger update if we actually loaded tiles
  if (anyTilesLoaded)
    this->update();
}

QString QMapTileWidget::getTileCacheFilePath(int x, int y, int z) const
{
  return QString("%1/%2/%3/%4.png").arg(this->tileCachePath).arg(z).arg(x).arg(y);
}

bool QMapTileWidget::loadTileFromDisk(int x, int y, int z)
{
  QString filePath = getTileCacheFilePath(x, y, z);
  if (!QFile::exists(filePath))
    return false;

  QPixmap pixmap(filePath);
  if (pixmap.isNull())
    return false;

  MapTile tile(x, y, z);
  QString key = tile.key();

  MapTile *newTile = new MapTile(x, y, z);
  newTile->pixmap = pixmap;
  newTile->loading = false;
  this->tileCache.insert(key, newTile, 1);

  return true;
}

void QMapTileWidget::saveTileToDisk(int x, int y, int z, const QPixmap &pixmap)
{
  QString filePath = getTileCacheFilePath(x, y, z);
  QFileInfo fileInfo(filePath);
  QDir().mkpath(fileInfo.absolutePath());

  pixmap.save(filePath, "PNG");
}

bool QMapTileWidget::loadTile(int x, int y, int z)
{
  MapTile tile(x, y, z);
  QString key = tile.key();

  // Check if tile is already in memory cache
  if (this->tileCache.contains(key))
    return false;

  // Try to load from disk cache first
  if (loadTileFromDisk(x, y, z)) {
    return true;
  }

  // Create placeholder tile
  MapTile *newTile = new MapTile(x, y, z);
  newTile->loading = true;
  this->tileCache.insert(key, newTile, 1);

  // Download tile from network
  QString url = this->tileServerUrl;
  url.replace("{z}", QString::number(z));
  url.replace("{x}", QString::number(x));
  url.replace("{y}", QString::number(y));

  QNetworkRequest request(url);
  request.setRawHeader("User-Agent", "TaxiVis/1.0");
  request.setAttribute(QNetworkRequest::User, key);

  this->networkManager->get(request);
  return false;
}


void QMapTileWidget::keyPressEvent(QKeyEvent *event)
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

  case Qt::Key_M:
    this->showMap(!this->showMapEnabled);
    break;

  case Qt::Key_F:
    this->showFps(!this->fpsRenderingLayer->isVisible());
    break;

  case Qt::Key_R:
    this->mapCenter = this->initialCenter;
    this->mapLevel = this->initialLevel;
    this->updateView();
    break;

  default:
    event->ignore();
    break;
  }
}

void QMapTileWidget::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
  this->lastButtonPressed = event->button();
  this->lastPos = event->screenPos();
}

QPointF QMapTileWidget::mapFromGeoLocation(QPointF geoCoords) const
{
  double unit = exp2((double)this->mapLevel);
  return QPointF((lon2x(geoCoords.y())-lon2x(this->mapCenter.y()))*unit+this->size().width()*0.5,
                 (lat2y(geoCoords.x())-lat2y(this->mapCenter.x()))*unit+this->size().height()*0.5);
}

QPointF QMapTileWidget::mapToGeoLocation(QPointF viewCoords) const
{
  double unit = exp2((double)this->mapLevel);
  return QPointF(y2lat((viewCoords.y()-this->size().height()*0.5)/unit+lat2y(this->mapCenter.x())),
                 x2lon((viewCoords.x()-this->size().width()*0.5)/unit+lon2x(this->mapCenter.y())));
}

void QMapTileWidget::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
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

void QMapTileWidget::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
  Q_UNUSED(event);
  this->lastButtonPressed = -1;
}

void QMapTileWidget::resizeEvent(QGraphicsSceneResizeEvent *event)
{
  QGraphicsWidget::resizeEvent(event);
  this->updateView();
}

void QMapTileWidget::wheelEvent(QGraphicsSceneWheelEvent *event)
{
  if (event->delta()>0)
    this->setZoomLevel(this->mapLevel+1);
  if (event->delta()<0)
    this->setZoomLevel(this->mapLevel-1);
}

void QMapTileWidget::initGL()
{
  if (this->glInitialized) return;
  RenderingLayerManager::iterator it;
  for (it=this->layers->begin(); it!=this->layers->end(); it++)
    it.value()->initGL();
  this->fpsRenderingLayer->initGL();
  if (this->mapWidget)
    this->mapWidget->initGL();
  this->glInitialized = true;
}

bool QMapTileWidget::sceneEvent(QEvent *event)
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

  return QGraphicsWidget::sceneEvent(event);
}

void QMapTileWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
  Q_UNUSED(option);
  Q_UNUSED(widget);

  static bool firstPaint = true;
  if (firstPaint) {
    qDebug() << "First paint: size=" << this->size() << "mapEnabled=" << this->showMapEnabled;
    firstPaint = false;
  }

  this->initGL();

  if (this->showMapEnabled) {
    // Ensure proper rendering mode
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, false);
    painter->setRenderHint(QPainter::SmoothPixmapTransform, true);

    // Fill background
    painter->fillRect(this->rect(), QColor(229, 227, 223));

    QPointF centerPixel = latLonToTilePixel(this->mapCenter.x(), this->mapCenter.y(), this->mapLevel);
    double offsetX = this->size().width() / 2.0 - fmod(centerPixel.x(), 256.0);
    double offsetY = this->size().height() / 2.0 - fmod(centerPixel.y(), 256.0);

    int centerTileX = long2tilex(this->mapCenter.y(), this->mapLevel);
    int centerTileY = lat2tiley(this->mapCenter.x(), this->mapLevel);

    int startTileX = centerTileX - (int)ceil(this->size().width() / 512.0) - 1;
    int startTileY = centerTileY - (int)ceil(this->size().height() / 512.0) - 1;
    int endTileX = centerTileX + (int)ceil(this->size().width() / 512.0) + 1;
    int endTileY = centerTileY + (int)ceil(this->size().height() / 512.0) + 1;

    int maxTile = (1 << this->mapLevel) - 1;

    for (int ty = startTileY; ty <= endTileY; ty++) {
      if (ty < 0 || ty > maxTile)
        continue;

      for (int tx = startTileX; tx <= endTileX; tx++) {
        int wrappedTx = tx;
        if (wrappedTx < 0) wrappedTx += (1 << this->mapLevel);
        if (wrappedTx > maxTile) wrappedTx -= (1 << this->mapLevel);

        QString key = QString("%1_%2_%3").arg(this->mapLevel).arg(wrappedTx).arg(ty);
        MapTile *tile = this->tileCache.object(key);

        // If tile not in memory cache, try to load from disk
        if (!tile) {
          if (loadTileFromDisk(wrappedTx, ty, this->mapLevel)) {
            tile = this->tileCache.object(key);
          }
        }

        if (tile && !tile->pixmap.isNull()) {
          double x = offsetX + (tx - centerTileX) * 256;
          double y = offsetY + (ty - centerTileY) * 256;

          // Draw tiles with reduced opacity for better data visibility
          painter->setOpacity(0.4);
          painter->drawPixmap(QPointF(x, y), tile->pixmap);
          painter->setOpacity(1.0);
        }
      }
    }

    painter->restore();
  }

  // Draw rendering layers
  bool glMode = false;
  RenderingLayerManager::iterator it;
  for (it=this->layers->begin(); it!=this->layers->end(); it++)
    glMode = it.value()->paint(painter, glMode);

  if (this->mapWidget)
    this->mapWidget->paintOverlay(painter);

  this->fpsRenderingLayer->paint(painter, glMode);
}

void QMapTileWidget::addRenderingLayer(RenderingLayer *layer, float depth)
{
  this->layers->add(layer, depth);
}

RenderingLayer* QMapTileWidget::getRenderingLayer(int stackingOrder)
{
  return this->layers->get(stackingOrder);
}

QList<RenderingLayer*> QMapTileWidget::getRenderingLayers()
{
  QList<RenderingLayer*> results;
  RenderingLayerManager::iterator it;
  for (it=this->layers->begin(); it!=this->layers->end(); it++)
    results.append(it.value());
  return results;
}
