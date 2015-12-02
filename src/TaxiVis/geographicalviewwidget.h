#ifndef GEOGRAPHICALVIEWWIDGET_H
#define GEOGRAPHICALVIEWWIDGET_H

#include "QMapWidget.hpp"
#include "KdTrip.hpp"
#include "RenderingLayer.hpp"
#include "SelectionGraph.h"
#include "timewidget.h"
#include <set>
#include <QDateTime>
#include <QHash>
#include <QVector>
#include <QStringList>

class GridMap;
class HeatMap;
class TripAnimation;
class TripLocation;
class ColorBar;
class Coordinator;

class GeographicalViewWidget : public QMapWidget
{
    Q_OBJECT
public:
    enum State{IDLE, PANNING, RECT_SELECTION, FREE_SELECTION, POLYGON_SELECTION, MOVE_SELECTION, EDIT_SELECTION, LINKING};
    enum SelectionMode {SINGLE, LINK};
  
private:

    State                      currentState;
    //
    QPainterPath               selectionPath;
    QPainterPath               projectedSelectionPath;//auxiliary that keep selection path in geocoords
    QLine                      linkingLine;
    SelectionGraphNode        *tailNode;

    QPoint                     basePosition;
    QPolygonF                  basePolygon;
    QPointF                    projectedBasePosition;
    QVector<QGraphicsItem*>    editingAnchors;
    KdTrip::TripSet           *selectedTrips;
    SelectionGraph*            selectionGraph;
    bool                       renderTrips;

    DateTimeList               selectionTimes;

    Selection::TYPE            selectionType; // START, END or START_END
    SelectionMode              selectionMode;

    TripAnimation             *layerAnimation;
    TripLocation              *layerLocation;
    GridMap                   *layerZipCode;
    GridMap                   *layerNeighborhood;
    HeatMap                   *layerHeatMap;

    Coordinator               *coordinator;
    bool                       selectionTimeVisible;
    QColor                     selectionTimeColor;

    bool                       queryDescriptionVisible;
    QStringList                queryDescription;
  
    //
    ColorBar                  *colorbar;

    //
    void         querySelectedData();
    void         renderSelections(QPainter *painter);
    Group        getAvailableGroup();
    QPainterPath convertToScreen(QPainterPath& path);
    QPolygonF    convertToScreen(QPolygonF& poly);
    void         pickSelection(SelectionGraphNode*&,QPointF);
    void         pickEdge(SelectionGraphEdge*&,QPointF);

    void         notifyCoordinatorViewChanged();
    void         notifyCoordinatorStepBack();
    void         notifyCoordinatorStepForward();
    void         notifyCoordinatorExportSelection();
    void         notifyCoordinatorKeyPress(QKeyEvent *event);

    void         donePolygonSelection();
    void         doneEditingSelection();
    void         updateEditingSelection();
    void         renderSelectionTime(QPainter *painter);
    void         renderQueryDescription(QPainter *painter);

private slots:
    void         updateEditingAnchors();

public:
    explicit GeographicalViewWidget(QWidget *parent = 0);
    ~GeographicalViewWidget();

    void setSelectedTripsRepository(KdTrip::TripSet *);
    void setSelectionGraph(SelectionGraph*);
    void setSelectionTime(QDateTime startT, QDateTime endT);
    void setSelectionType(Selection::TYPE type);
    void setSelectionMode(SelectionMode mode);
    void updateData();
    bool mergeSelections();
    bool unmergeSelections();

    KdTrip::TripSet * getSelectedTrips();
    QDateTime getSelectedStartTime();
    QDateTime getSelectedEndTime();
    void      setAnimationEnabled(bool b);
    SelectionGraph *getSelectionGraph();
    Selection::TYPE getSelectionType();
    QVector2D getScalarRange();
    ColorBar *getColorBar();
    void updateColorBar();
    void updateSelectionGraph(SelectionGraph*);

    void setSelectionTimes(const DateTimeList &list);
    DateTimeList getSelectionTimes();
    uint getSelectionDuration();
    void emitDatasetUpdated();
  
    bool isSelectionTimeVisible();
    void showSelectionTime(bool show);
    void setSelectionTimeColor(QColor color);

    bool isQueryDescriptionVisible();
    void showQueryDescription(bool show);
    void setQueryDescriptionColor(QColor color);

    void setCoordinator(Coordinator *c);
    void setQueryDescription(const QStringList &list);

signals:
    void mapSelectionChanged();
    void datasetUpdated();
    void stepBack();
    void stepForward();
protected:
    void loadFinished();
    void initGL();
    void paintOverlay(QPainter *painter);

    void drawArrow(QPainter *painter, QLineF line);

    //
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
};

#endif // GEOGRAPHICALVIEWWIDGET_H
