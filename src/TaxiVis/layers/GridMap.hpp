#ifndef GRID_MAP_HPP
#define GRID_MAP_HPP
#include "RenderingLayer.hpp"
#include "KdTrip.hpp"
#include "AsyncTask.hpp"
#include <set>
#include <vector>
#include <QList>
#include <QPolygonF>
#include <QVector2D>
#include <QOpenGLFramebufferObject>

class ColorScale;
class GeographicalViewWidget;
class Grid;
class GridCell;
class QGraphicsItem;

class GridMap : public QObject, public RenderingLayer
{
  Q_OBJECT
public:
  GridMap(GeographicalViewWidget *mw);
  virtual ~GridMap();
  void loadGrid(QString gridFile);
  void cancelComputation();
  bool computationBusy() const { return aggregationJob.isBusy(); }

  QColor borderColor();
  void   setBorderColor(QColor c);
  QColor fillColor();
  void   setFillColor(QColor c);

  void        setColorScale(ColorScale *scale);
  ColorScale *getColorScale();


  GridCell *highlightedCell();
  void      toggleHightlightedCellVisual();
  
  QVector2D getCellValueRange();

  virtual void setEnabled(bool r);
  virtual void initGL();
  virtual void render(QPainter *painter);

public slots:
  void updateData();
  void viewChanged(QPointF,int);
  void doneUpdating();

protected:
  enum AggregationKind { Count, Fare, Flow };
  struct Aggregation {
      std::vector<int> counts;
      std::vector<float> fares;
      std::vector<std::vector<float>> ratios;
      std::vector<KdTrip::TripSet> trips;
  };
  LatestTask<Aggregation> aggregationJob;
  virtual AggregationKind aggregationKind() const { return Count; }
  virtual void acceptAggregation(Aggregation &) {}
  void buildCells();
  void renderGL();
  void renderLabel(QPainter *painter);
  void computeVisualData();
  void updateCellVisualGeometry(GridCell *cell);

  virtual void renderPicking();
  virtual void aggregateBegin() {}
  virtual void aggregateUpdate(int, const KdTrip::Trip *) {}
  virtual void aggregateEnd() {}
  virtual void aggregateOutput(GridCell &) {}
  virtual void updateCellVisualContents(GridCell *);
  virtual QGraphicsItem *createCellVisual(GridCell *);

  Grid                   *grid;
  QVector2D               cellValueRange;

  bool                    dataReady;
  bool                    bufferDirty;
  bool                    visualDirty;

  GLBuffer                buffer[3];
  std::vector<float>      vertices;
  std::vector<int>        indices;
  std::vector<float>      triangles;

  PQOpenGLShaderProgram       shader;
  PQOpenGLFramebufferObject   fbo;

  QFont                   labelFont;
  QFontMetrics            labelMetrics;

  QColor                  colorBorder;
  QColor                  colorFill;
  ColorScale             *colorScale;

  int                     highlightId;
  
  typedef std::set<GridCell*> CellSet;
  CellSet                     visualCells;

  GeographicalViewWidget *geoWidget;
};

struct GridCell
{
  GridCell(): visual(NULL) {}
  QString          name;
  QList<QPolygonF> geometry;
  QRectF           boundingRect;

  int              id;
  int              strokeIndices[2];
  int              fillIndices[2];

  float            value;
  QString          label;
  QGraphicsItem   *visual;

  KdTrip::TripSet  trips;

  bool contains(const QPointF&p) const
  {
    if (!this->boundingRect.contains(p))
      return false;
    for (int i=0; i<this->geometry.size(); i++)
      if (this->geometry[i].containsPoint(p, Qt::OddEvenFill))
        return true;
    return false;
  }
};

class NumTripsGridMap : public GridMap
{
public:
  NumTripsGridMap(GeographicalViewWidget *gw);

protected:
  std::vector<int> counts;
  void acceptAggregation(Aggregation &a) override { counts=std::move(a.counts); }

  void aggregateBegin();
  void aggregateUpdate(int, const KdTrip::Trip *);
  void aggregateEnd();
  void aggregateOutput(GridCell &);
};

class FarePerMileGridMap : public GridMap
{
public:
  FarePerMileGridMap(GeographicalViewWidget *gw);

protected:
  std::vector<int>   counts;
  std::vector<float> fares;

  AggregationKind aggregationKind() const override { return Fare; }
  void acceptAggregation(Aggregation &a) override { counts=std::move(a.counts); fares=std::move(a.fares); }
  void aggregateBegin();
  void aggregateUpdate(int, const KdTrip::Trip *);
  void aggregateEnd();
  void aggregateOutput(GridCell &);
};

class PickupDropoffGridMap : public GridMap
{
public:
  PickupDropoffGridMap(GeographicalViewWidget *gw);

protected:
  std::vector<int> counts;
  std::vector< std::vector<float> > ratios;
  
  AggregationKind aggregationKind() const override { return Flow; }
  void acceptAggregation(Aggregation &a) override { counts=std::move(a.counts); ratios=std::move(a.ratios); }
  void renderPicking();
  void aggregateBegin();
  void aggregateUpdate(int, const KdTrip::Trip *);
  void aggregateEnd();
  void aggregateOutput(GridCell &);
};

#endif
