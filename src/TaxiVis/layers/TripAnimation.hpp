#ifndef TRIP_ANIMATION_HPP
#define TRIP_ANIMATION_HPP
#include "RenderingLayer.hpp"
#include <QFont>
#include <QVector2D>
#include <vector>

class GeographicalViewWidget;
class QOpenGLShaderProgram;
class QGraphicsProxyWidget;
class QProgressDialog;
namespace Ui
{
  class TripAnimationConfig;
  class TripAnimationToolBar;
};

class TripAnimation : public QObject, public RenderingLayer
{
  Q_OBJECT
public:
  TripAnimation(GeographicalViewWidget *mw);
  ~TripAnimation();

  void initGL();
  void setEnabled(bool r);
  bool isSupported() const;
  void render(QPainter *painter);

  bool isPlaying();
  void togglePlaying();

  void renderGL();
  void renderQt(QPainter *painter);

  qreal glyphSize();
  void  setGlyphSize(qreal size);

  qreal trailingPeriod();
  void setTrailingPeriod(qreal seconds);

  int   playSpeed();

  int   frameCount();
  int   currentFrame();

public slots:
  void play();
  void stop();
  void reset();
  void setPlaying(bool r);
  void showConfig(bool r);

  void startNextFrame();
  void updateData();

  void glyphSizeSliderChanged(int);
  void trailingPeriodSliderChanged(int);
  void setCurrentFrame(int frame);
  void setPlaySpeed(int speed);
  
protected:
  void buildAnimPath();
  void updatePathBuffers();
  void renderPaths();
  void paintCurrentTime(QPainter *painter);
  
  void setFrameCount(int cnt);

private:
  typedef QSharedPointer<QOpenGLShaderProgram> PQOpenGLShaderProgram;
  
  Ui::TripAnimationConfig  *uiConfig;
  Ui::TripAnimationToolBar *uiToolBar;

  bool playing;
  bool pathDataReady;
  bool pathBufferDirty;
  
  int       trafficTime;
  int       maxTrafficTime;
  uint64_t  globalTime;
  int       frameInterval;
  QVector2D glyphArea;
  GLBuffer  bufPath[2];
  
  int                            pathGeomVertexCount;
  std::vector<float>             pathVertices;
  std::vector<int>               pathIndices;
  std::vector<float>             pathWeights;
  std::vector<PQOpenGLShaderProgram> shaders;

  QFont                   font;
  GeographicalViewWidget *geoWidget;
  QProgressDialog        *progress;
  QWidget                *configWidget;
  QGraphicsProxyWidget   *config;
  QWidget                *toolBarWidget;
  QGraphicsProxyWidget   *toolBar;
};

#endif
