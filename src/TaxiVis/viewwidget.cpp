#include "viewwidget.h"
#include "ui_viewwidget.h"
#include "HistogramDialog.hpp"
#include "TemporalSeriesDialog.hpp"
#include "TimeExplorationDialog.hpp"
#include "global.h"
#include "QMapView.hpp"
#include "coordinator.h"
#include "timewidget.h"
#include "GroupRepository.h"

#include <fstream>
#include <iostream>

#include <QtCore>
#include <QFileDialog>

ViewWidget::ViewWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ViewWidget)
{
    ui->setupUi(this);

    //
    ui->geographicalView->setSelectedTripsRepository(&selectedTrips);
    ui->geographicalView->setSelectionGraph(&selectionGraph);
    ui->geographicalView->setSelectionTime(ui->timeSelectionWidget->getStartTime(),ui->timeSelectionWidget->getEndTime());
    ui->geographicalView->updateData();

    //
    ui->timeSeriesWidget->setSelectedTripsRepository(&selectedTrips);
    ui->timeSeriesWidget->setSelectionGraph(&selectionGraph);
    ui->timeSeriesWidget->setDateTimes(ui->timeSelectionWidget->getStartTime(),ui->timeSelectionWidget->getEndTime());
    ui->timeSeriesWidget->recomputePlots();

    //
    ui->scatterPlotWidget->setSelectedTripsRepository(&selectedTrips);
    ui->scatterPlotWidget->setSelectionGraph(&selectionGraph);
    ui->scatterPlotWidget->recomputePlots();

    //
    ui->histogramWidget->setSelectedTripsRepository(&selectedTrips);
    ui->histogramWidget->setSelectionGraph(&selectionGraph);
    ui->histogramWidget->recomputePlots();

    //
    ui->splitter->setStretchFactor(0, 1);
    ui->splitter->setStretchFactor(1, 2);

    //
    connect(ui->geographicalView,SIGNAL(datasetUpdated()),this,SLOT(geoWidgetUpdatedData()));

    // Autoexclude the Selection Modes
    QButtonGroup *buttonGroup = new QButtonGroup;
    buttonGroup->addButton(ui->pickupModeButton, 0);
    buttonGroup->addButton(ui->dropoffModeButton, 1);
    buttonGroup->addButton(ui->pickupAndDropoffButton, 2);
    buttonGroup->addButton(ui->linkModeButton, 3);
    buttonGroup->addButton(ui->mergeButton, 4);
    buttonGroup->addButton(ui->unmergeButton, 5);
    connect(buttonGroup, SIGNAL(buttonClicked(int)), this, SLOT(selectionModeChanges(int)));

    //
    connect(ui->timeSelectionWidget,SIGNAL(timeUpdated(QDateTime,QDateTime)),this,SLOT(updateTimes(QDateTime,QDateTime)));

    //
    connect(ui->timeSeriesWidget,SIGNAL(timeIntervalChanged(QDateTime,QDateTime)),this,SLOT(updateTimes(QDateTime,QDateTime)));

    //
    connect(ui->exportButton, SIGNAL(clicked()), this, SLOT(exportTrips()));

    //
    connect(ui->geographicalView,SIGNAL(stepBack()),this,SLOT(stepBack()));
    connect(ui->geographicalView,SIGNAL(stepForward()),this,SLOT(stepForward()));

    //
    connect(ui->exploreButton, SIGNAL(clicked()), this, SLOT(plotAllAttributes()));

    //
    connect(ui->timeSelectionWidget,SIGNAL(recurrentTimeUpdated(TimeWidget*)),this,SLOT(updateRecurrentTimes(TimeWidget*)));

    //
    connect(ui->timeSelectionWidget,SIGNAL(exploreInTime(const DateTimeList &)),this,SLOT(exploreInTime(const DateTimeList &)));

}

ViewWidget::~ViewWidget()
{
    if (Coordinator::instance()->containsView(this))
        Coordinator::instance()->removeView(this);
    delete ui;
}

TemporalSeriesPlotWidget *ViewWidget::timeSeriesWidget()
{
  return ui->timeSeriesWidget;
}

HistogramWidget *ViewWidget::histogramWidget()
{
   return ui->histogramWidget;
}

QMapWidget *ViewWidget::mapWidget()
{
    return ui->geographicalView;
}

void ViewWidget::stepBack(){
    ui->timeSelectionWidget->stepBack();
}

void ViewWidget::stepForward(){
    ui->timeSelectionWidget->stepForward();
}

void ViewWidget::setSelectionGraph(SelectionGraph *g){
std::cout << "Set selection graph" << std::endl;
}

void ViewWidget::geoWidgetUpdatedData(){
    //
    ui->timeSeriesWidget->setDateTimes(ui->geographicalView->getSelectedStartTime(),
                                       ui->geographicalView->getSelectedEndTime());
    //
    ui->timeSeriesWidget->recomputePlots();
    ui->histogramWidget->recomputePlots();
    ui->scatterPlotWidget->recomputePlots();
}

void ViewWidget::updateTimes(QDateTime start, QDateTime end){
    //qDebug() << "   After timeSelectionWidget update";
    ui->geographicalView->setSelectionTime(start,end);
    //qDebug() << "UpdateTimes " << start << "; " << end;
    ui->timeSelectionWidget->setTimes(start,end);
    //qDebug() << "   After geographicalWidget update";
    ui->geographicalView->updateData();
    //qDebug() << "   After geographicalWidget update data";
}

void ViewWidget::on_showMapButton_clicked(bool checked)
{
    ui->geographicalView->mapView()->showMap(checked);
}

void ViewWidget::on_showAnimationButton_clicked(bool checked)
{
    ui->geographicalView->setAnimationEnabled(checked);
}

void ViewWidget::exportTrips()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Export Trips"),
                                                    QDir::currentPath(),
                                                    tr("CSV (*.csv)"));
    if( !filename.isNull() ) {
        //
        Global* global = Global::getInstance();
        int numExtraFields = global->numExtraFields();

        QString header = "";
        QTextStream headerStream(&header);

        headerStream <<  "id_taxi, payment_type, pickup_time, dropoff_time, pickup_long, "
                << "pickup_lat, dropoff_long, dropoff_lat, distance (in 0.01 miles unit), fare_amount (cents), "
                << "surcharge (cents), mta_tax (cents), tip_amount (cents), tolls_amount (cents),passengers";

        for(int i = 0 ; i < numExtraFields ; ++i){
            ExtraField field = global->getExtraField(i);
            if(field.active){
              headerStream << "," << field.internalName;
            }

        }

        //
        std::ofstream out(filename.toAscii());
        out << header.toStdString() << "\n";

        KdTrip::TripSet *trips = ui->geographicalView->getSelectedTrips();
        KdTrip::TripSet::iterator it = trips->begin();

        for(; it!= trips->end(); ++it) {
          const KdTrip::Trip *trip = *it;

          //
          QString extraFieldsStr = "";
          QTextStream extraFieldStream(&extraFieldsStr);
          for(int i = 0 ; i < numExtraFields ; ++i){
              ExtraField field = global->getExtraField(i);
              if(field.active){
                u_int32_t value = getExtraFieldValue(trip,i);
                extraFieldStream << "," << value;
              }

          }

          //
            out << trip->id_taxi << ","
                << (int)trip->payment_type << ","
                << QDateTime::fromTime_t(trip->pickup_time).toString("MM/dd/yy hh:mm:ss").toStdString() << ","
                << QDateTime::fromTime_t(trip->dropoff_time).toString("MM/dd/yy hh:mm:ss").toStdString() << ","
                << trip->pickup_long << ","
                << trip->pickup_lat << ","
                << trip->dropoff_long << ","
                << trip->dropoff_lat << ","
                << trip->distance << ","
                << trip->fare_amount << ","
                << trip->surcharge << ","
                << trip->mta_tax << ","
                << trip->tip_amount << ","
                << trip->tolls_amount << ","
                << (int)trip->passengers
                << extraFieldsStr.toStdString() //either empty or starts with comman
                << std::endl;
        }
        out.close();
    }
}

void ViewWidget::selectionModeChanges(int mode)
{
    if (mode==0 || mode==1 || mode==2) { // Single mode selection
        ui->geographicalView->setSelectionType((Selection::TYPE)mode);
        ui->geographicalView->setSelectionMode(GeographicalViewWidget::SINGLE);
    } else if (mode==3) // linking two nodes
        ui->geographicalView->setSelectionMode(GeographicalViewWidget::LINK);
    else if(mode == 4){
        //merge
        bool needToUpdatePlot = ui->geographicalView->mergeSelections();
        if(needToUpdatePlot){
            ui->timeSeriesWidget->recomputePlots();
            ui->geographicalView->repaintContents();
            ui->histogramWidget->recomputePlots();
            ui->scatterPlotWidget->recomputePlots();
        }
    }
    else if(mode == 5){
        bool needToUpdatePlot = ui->geographicalView->unmergeSelections();
        if(needToUpdatePlot){
            ui->timeSeriesWidget->recomputePlots();
            ui->geographicalView->repaintContents();
            ui->histogramWidget->recomputePlots();
            ui->scatterPlotWidget->recomputePlots();
        }
    }
}

void ViewWidget::on_syncButton_clicked(bool checked)
{
    if (checked) Coordinator::instance()->addView(this);
    else Coordinator::instance()->removeView(this);
    Coordinator::instance()->notifyAll();
}

void ViewWidget::plotAllAttributes()
{
  if (this->ui->tabWidget->currentWidget()==this->ui->timeSeriesWidget) {
    int numBins = this->ui->timeSeriesWidget->getNumberOfBins();
    TemporalSeriesDialog *dialog = new TemporalSeriesDialog(this->ui->geographicalView, numBins);
    int result = dialog->exec();
    if (result==QDialog::Accepted) {
      this->updateTimes(dialog->startTime(), dialog->endTime());
    }
    delete dialog;
  }
  else {
    int numBins = this->ui->histogramWidget->getNumberOfBins();
    HistogramDialog *dialog = new HistogramDialog(this->ui->geographicalView, numBins);
    dialog->setAttribute(Qt::WA_DeleteOnClose, true);
    dialog->show();
  }
}

void ViewWidget::updateRecurrentTimes(TimeWidget *widget)
{
  this->ui->geographicalView->setSelectionTimes(widget->getSelectedRanges());
  this->ui->geographicalView->updateData();
}

void ViewWidget::exploreInTime(const DateTimeList &timeRanges)
{

  if (timeRanges.count()>7) {
    if (QMessageBox::question(this, "Parameter Exploration",
                              QString("There will be a lot of results, %1 cells. Do you still want to proceed?" )
                              .arg(timeRanges.count()),
                              QMessageBox::Yes | QMessageBox::No)!=QMessageBox::Yes)
      return;
  }

  QProgressDialog progress("Performing exploration...", QString(), 0, timeRanges.count(), this);
  progress.setMinimumDuration(0);

  TimeExplorationDialog *dialog = new TimeExplorationDialog(this);
  SelectionGraph selectionGraph, plotGraph;
  selectionGraph.assign(this->ui->geographicalView->getSelectionGraph());
  KdTrip::TripSet resultSet, plotSet;
  std::vector<KdTrip::Trip> plotTrips;
  KdTrip::TripSet::iterator tripIt;
  std::map<int,SelectionGraphNode*>::iterator beginNodeIterator;
  std::map<int,SelectionGraphNode*>::iterator   endNodeIterator;
  std::map<int,SelectionGraphEdge*>::iterator beginEdgeIterator;
  std::map<int,SelectionGraphEdge*>::iterator   endEdgeIterator;

  if(timeRanges.size() == 0){
      qDebug() << "EMPTY TIME RANGES";
      return;
  }

  DateTimePair baseRange = timeRanges.at(0);
  for (int i=1; i<timeRanges.count(); i++) {
    int len = timeRanges.at(i).first.secsTo(timeRanges.at(i).second);
    if (baseRange.first.secsTo(baseRange.second)<len)
      baseRange = timeRanges.at(i);
  }
  uint minPickup=UINT_MAX, maxDropoff = 0;
  for (int i=0; i<timeRanges.count(); i++) {
    progress.setValue(i);
    QApplication::processEvents();
    Global::getInstance()->queryData(&selectionGraph,
                                     timeRanges.at(i).first,
                                     timeRanges.at(i).second,
                                     resultSet);

    selectionGraph.getNodeIterator(beginNodeIterator,endNodeIterator);
    for(;beginNodeIterator != endNodeIterator ; ++beginNodeIterator) {
      SelectionGraphNode* node = beginNodeIterator->second;
      node->setGroup(GroupRepository::getInstance().getItem(i+1));
    }
    selectionGraph.getEdgeIterator(beginEdgeIterator,endEdgeIterator);
    for(; beginEdgeIterator != endEdgeIterator ; ++beginEdgeIterator) {
      SelectionGraphEdge* edge = beginEdgeIterator->second;
      edge->setGroup(GroupRepository::getInstance().getItem(i+1));
    }

    dialog->addGeoWidget(timeRanges.at(i).first,
                         timeRanges.at(i).second,
                         &selectionGraph,
                         resultSet);

    int delta = i==0?baseRange.second.secsTo(timeRanges.at(i).second):baseRange.first.secsTo(timeRanges.at(i).first);
    for (tripIt=resultSet.begin(); tripIt!=resultSet.end(); tripIt++) {
      KdTrip::Trip trip = *(*tripIt);
      trip.pickup_time -= delta;
      trip.dropoff_time -= delta;
      trip.pickup_lat = i;
      trip.pickup_long = i;
      plotTrips.push_back(trip);
      if (trip.pickup_time<minPickup) minPickup = trip.pickup_time;
      if (trip.dropoff_time>maxDropoff) maxDropoff = trip.dropoff_time;
    }
    QPainterPath path;
    path.addRect(i-0.5, i-0.5, 1, 1);
    Selection *sel = new Selection(path);
    sel->setType(Selection::START);
    SelectionGraphNode* node = plotGraph.addNode(sel);
    node->setGroup(GroupRepository::getInstance().getItem(i+1));
  }

  //
  for (size_t i=0; i<plotTrips.size(); i++)
    plotSet.insert(&plotTrips[i]);
  dialog->setPlotSelection(baseRange.first, baseRange.second, &plotGraph, &plotSet);

  //
  progress.setValue(progress.maximum());

  //
  dialog->exec();
  delete dialog;
}
