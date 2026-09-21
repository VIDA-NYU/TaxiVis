#ifndef SCATTERPLOTWIDGET_H
#define SCATTERPLOTWIDGET_H

#include <QWidget>
#include <set>
#include "SelectionGraph.h"
#include "KdTrip.hpp"
#include "AsyncTask.hpp"
#include "SelectionSnapshot.hpp"
#include "qcustomplot.h"

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
    void suspendComputation(bool value) { suspended=value; if (value) computeJob.cancel(); }
    bool computationBusy() const { return computeJob.isBusy(); }
    void cancelComputation() { computeJob.cancel(); }

private:
    bool suspended=false;
    Ui::ScatterPlotWidget *ui;

    //
    ScatterPlotAttributes attrib1;
    ScatterPlotAttributes attrib2;

    //
    KdTrip::TripSet      *selectedTrips;
    SelectionGraph       *selectionGraph;

    //
    struct ScatterData {
        std::map<Group, QSharedPointer<QCPGraphDataContainer>> groups;
        QCPRange xRange, yRange;
    };
    LatestTask<ScatterData> computeJob;
    void                  updatePlot();
    void                  updateAttributes();
    ScatterPlotAttributes getAttrib(QString);
    static QPointF getCoords(const KdTrip::Trip *, ScatterPlotAttributes attrib1, ScatterPlotAttributes attrib2);

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
