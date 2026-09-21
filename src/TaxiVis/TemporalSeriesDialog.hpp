#ifndef TEMPORAL_SERIES_DIALOG_H
#define TEMPORAL_SERIES_DIALOG_H

#include <QDialog>
#include "extendedplotwidget.h"
#include "KdTrip.hpp"
#include "SelectionGraph.h"

namespace Ui {
  class TemporalSeriesDialog;
}
class ExtendedPlotWidget;
class GeographicalViewWidget;

class TemporalSeriesDialog : public QDialog
{
  Q_OBJECT
public:
  TemporalSeriesDialog(GeographicalViewWidget *geoWidget, int numBins);
  ~TemporalSeriesDialog();

  QDateTime startTime();
  QDateTime endTime();

private:
  Ui::TemporalSeriesDialog *ui;
  KdTrip::TripSet tripSnapshot;
  SelectionGraph graphSnapshot;

  GeographicalViewWidget    *geoWidget;
  QList<ExtendedPlotWidget*> plots;

public slots:
  void xAxisRangeChanged(const QCPRange &newRange);
  void timeIntervalChanged(QDateTime, QDateTime);
};

#endif // TEMPORAL_SERIES_DIALOG_H
