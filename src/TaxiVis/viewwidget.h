#ifndef VIEWWIDGET_H
#define VIEWWIDGET_H

#include "KdTrip.hpp"
#include "SelectionGraph.h"
#include "timewidget.h"
#include "AsyncTask.hpp"
#include <QSaveFile>

#include <QWidget>
#include <QDateTime>

namespace Ui {
class ViewWidget;
}
class TemporalSeriesPlotWidget;
class QMapWidget;
class HistogramWidget;
class QProgressDialog;

class ViewWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ViewWidget(QWidget *parent = 0);
    ~ViewWidget();

    bool exportToFile(const QString &filename);
    bool exportBusy() const { return exportJob.isBusy(); }
    bool explorationBusy() const { return explorationJob.isBusy(); }
    void cancelExploration() { explorationJob.cancel(); }

    // Getters
    TemporalSeriesPlotWidget *timeSeriesWidget();
    HistogramWidget *histogramWidget();
    QMapWidget *mapWidget();

protected:
    void closeEvent(QCloseEvent *event) override;
private:
    QPointer<QProgressDialog> explorationProgress;
    Ui::ViewWidget *ui;

    struct ExplorationData {
        std::vector<KdTrip::TripSet> slices;
        KdTrip::TripSet plot;
    };
    LatestTask<ExplorationData> explorationJob;
    LatestTask<std::shared_ptr<QSaveFile>> exportJob;
    KdTrip::TripSet           selectedTrips;
    SelectionGraph            selectionGraph;

signals:
    void exportFinished(const QString &filename);
    void backgroundError(const QString &message);
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
