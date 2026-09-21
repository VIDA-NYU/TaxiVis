#include "HistogramDialog.hpp"
#include "ui_HistogramDialog.h"
#include "global.h"
#include <QDialogButtonBox>
#include <QPushButton>

HistogramDialog::HistogramDialog(GeographicalViewWidget *geo, int numBins)
    : QDialog(geo),
      ui(new Ui::HistogramDialog),
      geoWidget(geo)
{
  this->ui->setupUi(this);
  this->connect(this, SIGNAL(finished(int)), this, SLOT(onFinished(int)));
  
  this->trips = *this->geoWidget->getSelectedTrips();
  revision=geoWidget->dataRevision();
  filterJob.onBusy=[this](bool busy) {
      for (auto box : findChildren<QDialogButtonBox*>())
          for (auto button : box->buttons()) if (box->buttonRole(button)==QDialogButtonBox::AcceptRole) button->setEnabled(!busy);
  };
  selectionSnapshot.assign(geoWidget->getSelectionGraph());
  connect(geoWidget, &GeographicalViewWidget::queryBusyChanged, this, [this](bool busy) {
      if (busy) { stale=true; filterJob.cancel(); setEnabled(false); }
  });
  QDateTime startTime = this->geoWidget->getSelectedStartTime();
  QDateTime endTime = this->geoWidget->getSelectedEndTime();
  SelectionGraph *selectionGraph = &selectionSnapshot;
  QGridLayout *layout = this->ui->gridLayout;

  //add extra widgets to deal with the extra fields
  Global* global = Global::getInstance();
  int numExtraFields = global->numExtraFields();
  int numCols = layout->columnCount();
  int currentIndex = 0;
  for (int i=0; i<layout->count(); i++) {
      HistogramWidget *widget = dynamic_cast<HistogramWidget*>(layout->itemAt(i)->widget());
      if (widget) {
          ++currentIndex;
      }
  }

  //
  for(int i = 0 ; i < numExtraFields ; ++i){
      ExtraField field = global->getExtraField(i);
      if(field.active){
          HistogramWidget *widget = new HistogramWidget(this);
          int col = currentIndex % numCols;
          int row = currentIndex / numCols;

          layout->addWidget(widget,row,col,1,1);
          ++currentIndex;
      }
  }

  //
  for (int i=0; i<layout->count(); i++) {
    HistogramWidget *widget = dynamic_cast<HistogramWidget*>(layout->itemAt(i)->widget());
    if (widget) {
      QObjectList children = widget->children();
      for (int j=0; j<children.count(); j++)
        if (QWidget *w = dynamic_cast<QWidget*>(children[j])) {
          ExtendedHistogram *plot = dynamic_cast<ExtendedHistogram*>(w);
          if (!plot)
            w->hide();
          else
            this->plots.push_back(plot);
        }
      widget->setSelectionGraph(selectionGraph);
      widget->setSelectedTripsRepository(&this->trips);
      widget->setPlotAttribute((HistogramWidget::PlotAttribute)i);
      widget->setNumberOfBins(numBins);
      widget->recomputePlots();
    }
  }
  for (int i=0; i<this->plots.count(); i++) {
    this->connect(this->plots[i]->axisRect()->rangeDragAxis(Qt::Horizontal),
                  SIGNAL(rangeChanged(const QCPRange&)),
                  this, SLOT(xAxisRangeChanged(const QCPRange&)));
    this->connect(this->plots[i],
                  SIGNAL(updateSelection(QList<IntervalSelection>)),
                  this, SLOT(updateSelection(QList<IntervalSelection>)));
  }
}

HistogramDialog::~HistogramDialog()
{
  filterJob.cancel();
  for (auto widget : findChildren<HistogramWidget*>()) delete widget;
  delete this->ui;
}

void HistogramDialog::xAxisRangeChanged(const QCPRange &newRange)
{
  for (int i=0; i<this->plots.count(); i++)
    this->disconnect(this->plots[i]->axisRect()->rangeDragAxis(Qt::Horizontal),
                     SIGNAL(rangeChanged(const QCPRange&)),
                     this, SLOT(xAxisRangeChanged(const QCPRange&)));  
  for (int i=0; i<this->plots.count(); i++) {
    this->plots[i]->axisRect()->rangeDragAxis(Qt::Horizontal)->setRange(newRange);
    this->plots[i]->replot();
  }
  for (int i=0; i<this->plots.count(); i++) {
    this->connect(this->plots[i]->axisRect()->rangeDragAxis(Qt::Horizontal),
                  SIGNAL(rangeChanged(const QCPRange&)),
                  this, SLOT(xAxisRangeChanged(const QCPRange&)));
  }
}

void HistogramDialog::updateSelection(QList<IntervalSelection>)
{
  if (stale || geoWidget->queryBusy() || geoWidget->dataRevision()!=revision) return;
  std::vector<std::function<bool(const KdTrip::Trip*)>> filters;
  QStringList descriptions;
  for (auto widget : findChildren<HistogramWidget*>()) {
      filters.push_back(widget->filterSnapshot());
      auto description=widget->getAttributeDescription();
      if (!description.isEmpty()) descriptions << description;
  }
  const auto baseline=trips;
  filterJob.submit([baseline,filters](const Cancellation &cancel) {
      KdTrip::TripSet result; result.inheritOwners(baseline);
      size_t n=0;
      for (auto trip : baseline) {
          if ((n++ & 1023)==0) cancel.check();
          bool accepted=true;
          for (const auto &filter : filters) if (!filter(trip)) { accepted=false; break; }
          if (accepted) result.insert(trip);
      }
      return result;
  }, [this, descriptions](KdTrip::TripSet result) {
      if (stale || geoWidget->queryBusy() || geoWidget->dataRevision()!=revision) return;
      geoWidget->getSelectedTrips()->swap(result);
      geoWidget->setQueryDescription(descriptions);
      geoWidget->emitDatasetUpdated();
      revision=geoWidget->dataRevision();
      geoWidget->repaintContents();
  });
}

KdTrip::TripSet * HistogramDialog::selectedTrips()
{
  return &this->trips;
}

void HistogramDialog::onFinished(int result)
{
  filterJob.cancel();
  if (result==QDialog::Rejected && !stale && !geoWidget->queryBusy() && geoWidget->dataRevision()==revision) {
    this->geoWidget->getSelectedTrips()->swap(this->trips);
    this->geoWidget->setQueryDescription(QStringList());
    this->geoWidget->emitDatasetUpdated();
    this->geoWidget->repaintContents();
  }
}
