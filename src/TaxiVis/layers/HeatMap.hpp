#ifndef HEAT_MAP_HPP
#define HEAT_MAP_HPP
#include "RenderingLayer.hpp"
#include "KdTrip.hpp"
#include <set>
#include <vector>
#include <QtCore/QList>
#include <QtGui/QImage>
#include <QtGui/QVector2D>
#include <QtOpenGL/QGLFramebufferObject>

class ColorBar;
class ColorScale;
class GeographicalViewWidget;

class HeatMap : public QObject, public RenderingLayer
{
  Q_OBJECT
public:
  HeatMap(GeographicalViewWidget *mw);
  virtual ~HeatMap();

  void        setRegion(const QRectF &rect);
  void        setResolution(const QSize &size);
  void        setPointSize(int pointSize);
  
  float       getMaxValue();
  void        setMaxValue(float value);


  void        setColorScale(ColorScale *scale);
  ColorScale *getColorScale();

  void        setNormalized(bool n);
  bool        isNormalized();

  void        showColorBar(bool b);
  bool        isColorBarVisible();

  virtual void setEnabled(bool r);
  virtual void initGL();
  virtual void render(QPainter *painter);

signals:
  void maxValueUpdated(float value);

public slots:
  void updateData();

protected:
  void updateGrid();
  void renderGL();
  void computeVisualData();
  void buildHeatMapTexture();
  void updateColorBar();

  QSize                   resolution;
  QRectF                  region;
  QSizeF                  binSize;
  int                     pointSize;
  GLTexture               heatMapTexture;
  GLTexture               pointTexture;
  QImage                  textureImage;
  QImage                  pointImage;

  std::vector<int>        binCounts;
  int                     maxBinCount;
  float                   maxValue;

  bool                    initialized;
  bool                    normalized;
  bool                    dataReady;
  bool                    textureDirty;
  bool                    visualDirty;
  bool                    colorBarVisible;

  PQGLFramebufferObject   fbo;

  QFont                   labelFont;
  QFontMetrics            labelMetrics;
  ColorBar               *colorBar;
  ColorScale             *colorScale;

  GeographicalViewWidget *geoWidget;
};

#endif
