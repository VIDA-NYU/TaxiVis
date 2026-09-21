#ifndef TIME_EXPLORATION_DIALOG_H
#define TIME_EXPLORATION_DIALOG_H

#include <QDialog>
#include "geographicalviewwidget.h"

namespace Ui {
  class TimeExplorationDialog;
}

class TimeExplorationDialog : public QDialog
{
  Q_OBJECT
public:
  TimeExplorationDialog(QWidget *parent);
  ~TimeExplorationDialog();

  void addGeoWidget(QDateTime startTime, QDateTime endTime, SelectionGraph *graph, const KdTrip::TripSet &trips);
  void setPlotSelection(QDateTime startTime, QDateTime endTime, SelectionGraph *graph, KdTrip::TripSet *trips);
  void setMapView(QPointF center, int zoomLevel);

private:
  Ui::TimeExplorationDialog *ui;
  KdTrip::TripSet plotTrips;
  SelectionGraph plotGraph;
  
  QList<GeographicalViewWidget*> geoWidgets;
        
  Coordinator *coordinator;
  int valuesUpdated;
  
public slots:
  void maxValueUpdated(float value);
};

#endif // TIME_EXPLORATION_DIALOG_H
