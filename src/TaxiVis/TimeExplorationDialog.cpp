#include "TimeExplorationDialog.hpp"
#include "ui_TimeExplorationDialog.h"
#include "QMapView.hpp"
#include "coordinator.h"
#include "layers/HeatMap.hpp"
#include "GroupRepository.h"

TimeExplorationDialog::TimeExplorationDialog(QWidget *parent)
    : QDialog(parent),
      ui(new Ui::TimeExplorationDialog),
      valuesUpdated(0)
{
  this->coordinator = new Coordinator(this);
  this->ui->setupUi(this);
}

TimeExplorationDialog::~TimeExplorationDialog()
{
  for (int i=0; i<this->geoWidgets.count(); i++) {
    delete this->geoWidgets.at(i)->getSelectedTrips();
    delete this->geoWidgets.at(i)->getSelectionGraph();
  }
  delete this->ui;
  delete this->coordinator;
}

void TimeExplorationDialog::addGeoWidget(QDateTime startTime, QDateTime endTime, SelectionGraph *graph, const KdTrip::TripSet &inTrips)
{
  KdTrip::TripSet *trips = new KdTrip::TripSet(inTrips);
  SelectionGraph *selectionGraph = new SelectionGraph();
  selectionGraph->assign(graph);
  
  GeographicalViewWidget *geoWidget = new GeographicalViewWidget(this);
  this->geoWidgets.append(geoWidget);  
  this->ui->mainLayout->addWidget(geoWidget, 1);
  this->coordinator->addMapWidget(geoWidget);
  geoWidget->setCoordinator(this->coordinator);
  geoWidget->setSelectionTime(startTime, endTime);
  geoWidget->setSelectionTimeColor(GroupRepository::getInstance().getQColor(this->geoWidgets.count()));
  geoWidget->setSelectedTripsRepository(trips);
  geoWidget->setSelectionGraph(selectionGraph);
  geoWidget->showSelectionTime(true);
  
  HeatMap *hm = dynamic_cast<HeatMap*>(geoWidget->mapView()->getRenderingLayer(0));
  hm->showColorBar(this->geoWidgets.count()==1);
  hm->setEnabled(true);
  this->connect(hm, SIGNAL(maxValueUpdated(float)), this, SLOT(maxValueUpdated(float)));
  
  geoWidget->mapView()->getRenderingLayer(1)->setEnabled(false);
  
  geoWidget->emitDatasetUpdated();  
}

void TimeExplorationDialog::setPlotSelection(QDateTime startTime, QDateTime endTime, SelectionGraph *graph, KdTrip::TripSet *inTrips)
{ 
  //
  this->ui->timeSeriesWidget->setSelectedTripsRepository(inTrips);
  this->ui->timeSeriesWidget->setSelectionGraph(graph);
  this->ui->timeSeriesWidget->setDateTimes(startTime, endTime);
  this->ui->timeSeriesWidget->recomputePlots();

  //
  this->ui->scatterPlotWidget->setSelectedTripsRepository(inTrips);
  this->ui->scatterPlotWidget->setSelectionGraph(graph);
  this->ui->scatterPlotWidget->recomputePlots();

  //
  this->ui->histogramWidget->setSelectedTripsRepository(inTrips);
  this->ui->histogramWidget->setSelectionGraph(graph);
  this->ui->histogramWidget->recomputePlots();
}

void TimeExplorationDialog::maxValueUpdated(float value)
{
  valuesUpdated = (valuesUpdated+1)%this->geoWidgets.count();
  if (!valuesUpdated) {
    float maxValue = 0;
    for (int i=0; i<this->geoWidgets.count(); i++) {
      HeatMap *hm = dynamic_cast<HeatMap*>(this->geoWidgets.at(i)->mapView()->getRenderingLayer(0));
      if (hm->getMaxValue()>maxValue)
        maxValue = hm->getMaxValue();
    }
    for (int i=0; i<this->geoWidgets.count(); i++) {
      HeatMap *hm = dynamic_cast<HeatMap*>(this->geoWidgets.at(i)->mapView()->getRenderingLayer(0));
      hm->setMaxValue(maxValue);
      if (hm->isNormalized())
        this->geoWidgets.at(i)->repaintContents();
    }
  }
}
