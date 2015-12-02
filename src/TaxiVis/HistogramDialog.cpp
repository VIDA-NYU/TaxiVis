#include "HistogramDialog.hpp"
#include "ui_HistogramDialog.h"
#include "global.h"

HistogramDialog::HistogramDialog(GeographicalViewWidget *geo, int numBins)
    : QDialog(geo),
      ui(new Ui::HistogramDialog),
      geoWidget(geo)
{
  this->ui->setupUi(this);
  this->connect(this, SIGNAL(finished(int)), this, SLOT(onFinished(int)));
  
  this->trips = *this->geoWidget->getSelectedTrips();
  QDateTime startTime = this->geoWidget->getSelectedStartTime();
  QDateTime endTime = this->geoWidget->getSelectedEndTime();
  SelectionGraph *selectionGraph = this->geoWidget->getSelectionGraph();
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
    this->connect(this->plots[i]->rangeDragAxis(Qt::Horizontal),
                  SIGNAL(rangeChanged(const QCPRange&)),
                  this, SLOT(xAxisRangeChanged(const QCPRange&)));
    this->connect(this->plots[i],
                  SIGNAL(updateSelection(QList<IntervalSelection>)),
                  this, SLOT(updateSelection(QList<IntervalSelection>)));
  }
}

HistogramDialog::~HistogramDialog()
{
  delete this->ui;
}

void HistogramDialog::xAxisRangeChanged(const QCPRange &newRange)
{
  for (int i=0; i<this->plots.count(); i++)
    this->disconnect(this->plots[i]->rangeDragAxis(Qt::Horizontal),
                     SIGNAL(rangeChanged(const QCPRange&)),
                     this, SLOT(xAxisRangeChanged(const QCPRange&)));  
  for (int i=0; i<this->plots.count(); i++) {
    this->plots[i]->rangeDragAxis(Qt::Horizontal)->setRange(newRange);
    this->plots[i]->replot();
  }
  for (int i=0; i<this->plots.count(); i++) {
    this->connect(this->plots[i]->rangeDragAxis(Qt::Horizontal),
                  SIGNAL(rangeChanged(const QCPRange&)),
                  this, SLOT(xAxisRangeChanged(const QCPRange&)));
  }
}

void HistogramDialog::updateSelection(QList<IntervalSelection>)
{
  int selectionCount = 0;
  for (int i=0; i<this->plots.count(); i++) {
    selectionCount += this->plots.at(i)->getSelections().count();
  }
  KdTrip::TripSet out = this->trips;
  QGridLayout *layout = this->ui->gridLayout;
  QStringList descList;
  for (int i=0; i<layout->count(); i++) {
    HistogramWidget *widget = dynamic_cast<HistogramWidget*>(layout->itemAt(i)->widget());
    if (widget) {
      widget->joinSelectedTrips(&out);
      QString desc = widget->getAttributeDescription();
      if (!desc.isEmpty())
        descList << desc;
    }
  }
  this->geoWidget->getSelectedTrips()->swap(out);
  this->geoWidget->setQueryDescription(descList);
  this->geoWidget->emitDatasetUpdated();
  this->geoWidget->repaintContents();
}

KdTrip::TripSet * HistogramDialog::selectedTrips()
{
  return &this->trips;
}

void HistogramDialog::onFinished(int result)
{
  if (result==QDialog::Rejected) {
    this->geoWidget->getSelectedTrips()->swap(this->trips);
    this->geoWidget->setQueryDescription(QStringList());
    this->geoWidget->emitDatasetUpdated();
    this->geoWidget->repaintContents();
  }
}
