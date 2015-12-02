#ifndef HISTOGRAM_DIALOG_H
#define HISTOGRAM_DIALOG_H

#include <QtGui/QDialog>
#include "extendedhistogram.h"
#include "geographicalviewwidget.h"

namespace Ui {
  class HistogramDialog;
}
class ExtendedPlotWidget;
class GeographicalViewWidget;

class HistogramDialog : public QDialog
{
  Q_OBJECT
public:
  HistogramDialog(GeographicalViewWidget *geoWidget, int numBins);
  ~HistogramDialog();
  KdTrip::TripSet * selectedTrips();

private:
  Ui::HistogramDialog *ui;

  GeographicalViewWidget         *geoWidget;
  QList<ExtendedHistogram*>       plots;
  KdTrip::TripSet trips;
                                       
public slots:
  void xAxisRangeChanged(const QCPRange &newRange);
  void updateSelection(QList<IntervalSelection>);
  void onFinished(int result);
};

#endif // HISTOGRAM_DIALOG_H
