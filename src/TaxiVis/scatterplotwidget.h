#ifndef SCATTERPLOTWIDGET_H
#define SCATTERPLOTWIDGET_H

#include <QWidget>
#include <set>
#include "SelectionGraph.h"
#include "KdTrip.hpp"

namespace Ui {
class ScatterPlotWidget;
}

class ScatterPlotWidget : public QWidget
{
    Q_OBJECT
public:
    enum ScatterPlotAttributes{FARE_AMOUNT, TIP_AMOUNT, DISTANCE, DURATION, TOLL_AMOUNT, AVG_SPEED,
                              TIME_OF_DAY, FIELD1, FIELD2, FIELD3, FIELD4};

public:
    explicit ScatterPlotWidget(QWidget *parent = 0);
    ~ScatterPlotWidget();

    void setSelectedTripsRepository(KdTrip::TripSet *);
    void setSelectionGraph(SelectionGraph*);
    void recomputePlots();

private:
    Ui::ScatterPlotWidget *ui;

    //
    ScatterPlotAttributes attrib1;
    ScatterPlotAttributes attrib2;

    //
    KdTrip::TripSet      *selectedTrips;
    SelectionGraph       *selectionGraph;

    //
    void                  updatePlot();
    void                  updateAttributes();
    ScatterPlotAttributes getAttrib(QString);
    QPointF               getCoords(const KdTrip::Trip *);

    bool tripSatisfiesEdge(const KdTrip::Trip *trip, SelectionGraphEdge* edge);
    bool tripSatisfiesConstraints(const KdTrip::Trip *trip,
                                  std::vector<SelectionGraphNode*> groupNodeConstraints,
                                  std::vector<SelectionGraphEdge*> groupEdgeConstraints);

private slots:
    void setAttribute1(QString);
    void setAttribute2(QString);
    void mousePress();
    void mouseWheel();
};

#endif // SCATTERPLOTWIDGET_H
