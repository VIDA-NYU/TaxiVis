#ifndef HEAT_MAP_HPP
#define HEAT_MAP_HPP
#include "RenderingLayer.hpp"
#include "KdTrip.hpp"
#include "AsyncTask.hpp"
#include <set>
#include <vector>
#include <QList>
#include <QImage>
#include <QVector2D>
#include <QOpenGLFramebufferObject>

class ColorBar;
class ColorScale;
class GeographicalViewWidget;

class HeatMap : public QObject, public RenderingLayer
{
  Q_OBJECT
public:
  HeatMap(GeographicalViewWidget *mw);
  virtual ~HeatMap();

  void cancelComputation() { countJob.cancel(); textureJob.cancel(); dataReady=false; visualDirty=true; }
  bool computationBusy() const { return countJob.isBusy() || textureJob.isBusy(); }
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

  struct Counts { std::shared_ptr<std::vector<int>> bins; int maximum=0; };
  LatestTask<Counts> countJob;
  LatestTask<QImage> textureJob;
  QSize                   resolution;
  QRectF                  region;
  QSizeF                  binSize;
  int                     pointSize;
  GLTexture               heatMapTexture;
  GLTexture               pointTexture;
  QImage                  textureImage;
  QImage                  pointImage;

  std::shared_ptr<std::vector<int>> binCounts;
  int                     maxBinCount=0;
  float                   maxValue;

  bool                    initialized;
  bool                    normalized;
  bool                    dataReady;
  bool                    textureDirty;
  bool                    visualDirty;
  bool                    colorBarVisible;

  PQOpenGLFramebufferObject   fbo;

  QFont                   labelFont;
  QFontMetrics            labelMetrics;
  ColorBar               *colorBar;
  ColorScale             *colorScale;

  GeographicalViewWidget *geoWidget;
};

#endif
