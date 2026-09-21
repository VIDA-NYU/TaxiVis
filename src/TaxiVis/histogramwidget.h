#ifndef HISTOGRAMWIDGET_H
#define HISTOGRAMWIDGET_H

#include <QWidget>
#include <limits>
//
#include "qcustomplot.h"
#include "KdTrip.hpp"
#include "SelectionGraph.h"
#include "AsyncTask.hpp"
#include "SelectionSnapshot.hpp"

namespace Ui {
class HistogramWidget;
}
class Coordinator;

struct HistBin{
    float minBin;
    float maxBin;
    float freq;
    HistBin():
        minBin(std::numeric_limits<float>::max()),
        maxBin(std::numeric_limits<float>::min()),
        freq(0.0f)
    {}
};

class HistogramWidget : public QWidget
{
    Q_OBJECT
public:
    enum PlotAttribute{
        FARE_AMOUNT, TIP_AMOUNT, DISTANCE, DURATION, FIELD1, FIELD2, FIELD3, FIELD4
    };

    explicit HistogramWidget(QWidget *parent = 0);
    ~HistogramWidget();

    void setSelectedTripsRepository(KdTrip::TripSet *);
    void setSelectionGraph(SelectionGraph*);
    void setPlotAttribute(HistogramWidget::PlotAttribute pAttrib);
    int  getNumberOfBins();
    void joinSelectedTrips(KdTrip::TripSet *trips);
    std::function<bool(const KdTrip::Trip*)> filterSnapshot() const;

    QString getAttributeDescription();

    float yMin() { return _yMin; }
    float yMax() { return _yMax; }
    PlotAttribute plotAttribute() {return _plotAttribute;}

private:
    bool suspended=false;
    Ui::HistogramWidget *ui;

    //
    KdTrip::TripSet *selectedTrips=nullptr;
    SelectionGraph  *selectionGraph=nullptr;

    //
    Coordinator    *coordinator;

    //
    std::map<Group, std::map<PlotAttribute, std::vector<HistBin> > > groupHistograms;
    std::map<PlotAttribute, std::pair<float,float> >       histogramDataBounds;
    PlotAttribute                                          _plotAttribute;
    int                                                    numberOfBins;
    float                                                  _yMin;
    float                                                  _yMax;

    //
    std::map<Group,QCPBars*> groupPlots;

    //
    struct HistogramData {
        std::map<Group, std::map<PlotAttribute, std::vector<HistBin>>> groups;
        std::map<PlotAttribute, std::pair<float,float>> bounds;
    };
    LatestTask<HistogramData> computeJob;
public:
    void suspendComputation(bool value) { suspended=value; if (value) computeJob.cancel(); }
    bool computationBusy() const { return computeJob.isBusy(); }
    void cancelComputation() { computeJob.cancel(); }
private:
    bool tripSatisfiesEdge(const KdTrip::Trip *trip, SelectionGraphEdge* edge);
    bool tripSatisfiesConstraints(const KdTrip::Trip *trip,
                                  std::vector<SelectionGraphNode*> groupNodeConstraints,
                                  std::vector<SelectionGraphEdge*> groupEdgeConstraints);
    void updateControlValues();

    //
    static float getTripValue(const KdTrip::Trip *,PlotAttribute);

public:
    void updatePlots();
    void recomputePlots();
    void updateYRange(float min, float max);

public slots:
    void updateNumBins();
    void changeHistAttribute(QString);
    void setNumberOfBins(int);

private slots:
    void mousePress(QMouseEvent*);
    void mouseDouble(QMouseEvent*);
    void mouseWheel();
};

#endif // HISTOGRAMWIDGET_H
