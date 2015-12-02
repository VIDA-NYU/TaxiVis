#ifndef VIEWWIDGET_H
#define VIEWWIDGET_H

#include "KdTrip.hpp"
#include "SelectionGraph.h"
#include "timewidget.h"

#include <QWidget>
#include <QDateTime>

namespace Ui {
class ViewWidget;
}
class TemporalSeriesPlotWidget;
class QMapWidget;
class HistogramWidget;

class ViewWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ViewWidget(QWidget *parent = 0);
    ~ViewWidget();

    // Getters
    TemporalSeriesPlotWidget *timeSeriesWidget();
    HistogramWidget *histogramWidget();
    QMapWidget *mapWidget();

private:
    Ui::ViewWidget *ui;

    KdTrip::TripSet           selectedTrips;
    SelectionGraph            selectionGraph;

public slots:
    void geoWidgetUpdatedData();
    void updateTimes(QDateTime,QDateTime);
    void stepBack();
    void stepForward();
    void setSelectionGraph(SelectionGraph* g);
    void plotAllAttributes();
    void updateRecurrentTimes(TimeWidget*widget);

private slots:
    void on_showMapButton_clicked(bool checked);
    void selectionModeChanges(int mode);
    void on_syncButton_clicked(bool checked);
    void on_showAnimationButton_clicked(bool checked);
    void exportTrips();
    void exploreInTime(const DateTimeList &timeRange);
};

#endif // VIEWWIDGET_H
