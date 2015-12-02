#ifndef PLOTWIDGET_H
#define PLOTWIDGET_H

#include <QWidget>
#include <QDateTime>
#include <set>
#include <map>
#include "KdTrip.hpp"
#include "Group.h"
#include "SelectionGraph.h"

namespace Ui {
class PlotWidget;
}

class Coordinator;

struct HourSlot {
    uint64_t startTime;
    uint64_t endTime;
    uint64_t  num_trips;
    uint64_t  num_taxis;
    float     sum_distance;
    float     sum_tips;
    float     sum_fare_amount;
    float     sum_tools_amount;
    float     sum_duration;
    float     sum_avg_speed;
    //
    float     sum_field1;
    float     sum_field2;
    float     sum_field3;
    float     sum_field4;
    HourSlot(uint64_t startTime, uint64_t endTime):
        startTime(startTime),
        endTime(endTime),
        num_trips(0),
        num_taxis(0),
        sum_distance(0),
        sum_tips(0),
        sum_fare_amount(0),
        sum_tools_amount(0),
        sum_duration(0),
        sum_avg_speed(0),
        sum_field1(0),
        sum_field2(0),
        sum_field3(0),
        sum_field4(0)
    {}
    void reset(uint64_t startTime, uint64_t endTime)
    {
        this->startTime = startTime;
        this->endTime = endTime;
        this->num_trips = 0;
        this->num_taxis = 0;
        this->sum_distance = 0;
        this->sum_tips = 0;
        this->sum_fare_amount = 0;
        this->sum_tools_amount = 0;
        this->sum_duration = 0;
        this->sum_avg_speed = 0;
        //
        this->sum_field1 = 0;
        this->sum_field2 = 0;
        this->sum_field3 = 0;
        this->sum_field4 = 0;
    }
    inline void update(const KdTrip::Trip *trip){
        this->num_trips        += 1;
        this->sum_distance     += trip->distance;
        this->sum_fare_amount  += trip->fare_amount;
        this->sum_tips         += trip->tip_amount;
        this->sum_tools_amount += trip->tolls_amount/100.f;
        //
        this->sum_field1       += trip->field1;
        this->sum_field2       += trip->field2;
        this->sum_field3       += trip->field3;
        this->sum_field4       += trip->field4;
        //
        int32_t tripDuration = (trip->dropoff_time- trip->pickup_time);
        if(tripDuration > 0){
            this->sum_duration  += tripDuration;
            this->sum_avg_speed += (trip->distance/(1.0f*tripDuration));
        }
    }
};

class TemporalSeriesPlotWidget : public QWidget
{
    Q_OBJECT
public:
    enum PlotAttribute{
        NUMBER_OF_TRIPS, FARE_AMOUNT, TIP_AMOUNT, TOTAL_AMOUNT,
        FARE_PER_TRIP, FARE_PER_MILE, TIP_PER_TRIP, TIP_PER_MILE,
        NUM_TAXIS,TOLL_AMOUNT,TOLL_PER_TRIP, DURATION_PER_TRIP, DISTANCE_PER_TRIP, AVG_SPEED_PER_TRIP,
        FIELD1,FIELD2,FIELD3,FIELD4
    };

public:
    explicit TemporalSeriesPlotWidget(QWidget *parent = 0);
    ~TemporalSeriesPlotWidget();

    void setSelectedTripsRepository(KdTrip::TripSet *);

    float yMin() { return _yMin; }
    float yMax() { return _yMax; }
    PlotAttribute plotAttribute() {return _plotAttribute;}

private:
    Ui::PlotWidget *ui;
    KdTrip::TripSet*selectedTrips;

    //
    SelectionGraph* selectionGraph;
    QDateTime       startTime;
    QDateTime       endTime;

    //
    Coordinator    *coordinator;

    //
    std::map<Group, std::vector<HourSlot> > groupPlots;
    PlotAttribute _plotAttribute;
    int numBins;
    float _yMin;
    float _yMax;


    void computePlots();
    void setNumBins(int n);

    //
    bool tripSatisfiesEdge(const KdTrip::Trip *trip, SelectionGraphEdge* edge);
    bool tripSatisfiesConstraints(const KdTrip::Trip *trip,
                                  std::vector<SelectionGraphNode*> groupNodeConstraints,
                                  std::vector<SelectionGraphEdge*> groupEdgeConstraints);

public:
    void recomputePlots();
    void updatePlots();
    void setSelectionGraph(SelectionGraph*);
    void setDateTimes(QDateTime,QDateTime);
    void setPlotAttribute(TemporalSeriesPlotWidget::PlotAttribute pAttrib);
    void setNumberOfBins(int n);
    int  getNumberOfBins();
    void setCoordinator(Coordinator *c);

public slots:
    void updateNumBins();
    void updatePlotAttrib(QString);
    void updateYRange(float min, float max);
    void selectionChanged(QDateTime,QDateTime);
private slots:
    void mousePress();
    void mouseWheel();
signals:
    void timeIntervalChanged(QDateTime,QDateTime);
};

#endif // PLOTWIDGET_H
