#include "viewwidget.h"
#include "ui_viewwidget.h"
#include "HistogramDialog.hpp"
#include "TemporalSeriesDialog.hpp"
#include "TimeExplorationDialog.hpp"
#include "global.h"
#include "QMapTileWidget.hpp"
#include "coordinator.h"
#include "timewidget.h"
#include "GroupRepository.h"

#include <fstream>
#include <iostream>

#include <QtCore>
#include <QFileDialog>
#include <QButtonGroup>
#include <QMessageBox>
#include <QProgressDialog>

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
    connect(ui->geographicalView, &GeographicalViewWidget::queryBusyChanged, this, [this](bool busy) {
        ui->exportButton->setEnabled(!busy && ui->geographicalView->hasCurrentSelection() && !exportJob.isBusy());
        ui->exploreButton->setEnabled(!busy && ui->geographicalView->hasCurrentSelection());
        ui->geographicalView->setToolTip(busy ? tr("Updating selection…") : QString());
        ui->timeSeriesWidget->suspendComputation(busy || !ui->geographicalView->hasCurrentSelection());
        ui->histogramWidget->suspendComputation(busy || !ui->geographicalView->hasCurrentSelection());
        ui->scatterPlotWidget->suspendComputation(busy || !ui->geographicalView->hasCurrentSelection());
        ui->tabWidget->setEnabled(!busy && ui->geographicalView->hasCurrentSelection());
    });
    connect(ui->geographicalView, &GeographicalViewWidget::queryFailed, this, &ViewWidget::backgroundError);
    exportJob.onBusy = [this](bool busy) { ui->exportButton->setEnabled(!busy && !ui->geographicalView->queryBusy()); };
    exportJob.onError = [this](const QString &error) { emit backgroundError(error); };
    explorationJob.onError = exportJob.onError;
    connect(this, &ViewWidget::backgroundError, this, [this](const QString &error) {
        auto box = new QMessageBox(QMessageBox::Warning, tr("Background operation failed"), error, QMessageBox::Ok, this);
        box->setAttribute(Qt::WA_DeleteOnClose);
        box->open();
    });
    ui->exportButton->setEnabled(!ui->geographicalView->queryBusy());
    ui->exploreButton->setEnabled(!ui->geographicalView->queryBusy());
    if (ui->geographicalView->queryBusy()) {
        ui->timeSeriesWidget->suspendComputation(true);
        ui->histogramWidget->suspendComputation(true);
        ui->scatterPlotWidget->suspendComputation(true);
        ui->tabWidget->setEnabled(false);
    }


    // Autoexclude the Selection Modes
    QButtonGroup *buttonGroup = new QButtonGroup;
    buttonGroup->addButton(ui->pickupModeButton, 0);
    buttonGroup->addButton(ui->dropoffModeButton, 1);
    buttonGroup->addButton(ui->pickupAndDropoffButton, 2);
    buttonGroup->addButton(ui->linkModeButton, 3);
    buttonGroup->addButton(ui->mergeButton, 4);
    buttonGroup->addButton(ui->unmergeButton, 5);
    connect(buttonGroup, SIGNAL(idClicked(int)), this, SLOT(selectionModeChanges(int)));

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

void ViewWidget::closeEvent(QCloseEvent *event)
{
    explorationJob.cancel(); exportJob.cancel();
    ui->geographicalView->cancelQuery();
    ui->timeSeriesWidget->cancelComputation();
    ui->histogramWidget->cancelComputation();
    ui->scatterPlotWidget->cancelComputation();
    QWidget::closeEvent(event);
}

ViewWidget::~ViewWidget()
{
    if (Coordinator::instance()->containsView(this))
        Coordinator::instance()->removeView(this);
    explorationJob.cancel();
    exportJob.cancel();
    delete ui->geographicalView;
    delete ui->timeSeriesWidget;
    delete ui->histogramWidget;
    delete ui->scatterPlotWidget;
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
    ui->timeSeriesWidget->suspendComputation(false);
    ui->histogramWidget->suspendComputation(false);
    ui->scatterPlotWidget->suspendComputation(false);
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
    if (ui->geographicalView->queryBusy() || !ui->geographicalView->hasCurrentSelection() || exportJob.isBusy()) return;
    const auto filename = QFileDialog::getSaveFileName(this, tr("Export Trips"), QDir::currentPath(), tr("CSV (*.csv)"));
    if (!filename.isEmpty()) exportToFile(filename);
}

bool ViewWidget::exportToFile(const QString &filename)
{
    if (filename.isEmpty() || ui->geographicalView->queryBusy() || !ui->geographicalView->hasCurrentSelection() || exportJob.isBusy()) return false;
    const auto trips = selectedTrips;
    QString header = "id_taxi, payment_type, pickup_time, dropoff_time, pickup_long, pickup_lat, dropoff_long, dropoff_lat, distance (in 0.01 miles unit), fare_amount (cents), surcharge (cents), mta_tax (cents), tip_amount (cents), tolls_amount (cents),passengers";
    std::vector<int> fields;
    auto global = Global::getInstance();
    for (int i=0; i<global->numExtraFields(); ++i) {
        auto field = global->getExtraField(i);
        if (field.active) { header += "," + field.internalName; fields.push_back(i); }
    }
    exportJob.submit([trips, filename, header, fields](const Cancellation &cancel) {
        auto file = std::make_shared<QSaveFile>(filename);
        if (!file->open(QIODevice::WriteOnly)) throw std::runtime_error(file->errorString().toStdString());
        QTextStream out(file.get());
        out << header << '\n';
        size_t n=0;
        for (auto trip : trips) {
            if ((n++ & 1023)==0) cancel.check();
            out << trip->id_taxi << ',' << int(trip->payment_type) << ','
                << QDateTime::fromSecsSinceEpoch(trip->pickup_time).toString("MM/dd/yy hh:mm:ss") << ','
                << QDateTime::fromSecsSinceEpoch(trip->dropoff_time).toString("MM/dd/yy hh:mm:ss") << ','
                << trip->pickup_long << ',' << trip->pickup_lat << ',' << trip->dropoff_long << ',' << trip->dropoff_lat << ','
                << trip->distance << ',' << trip->fare_amount << ',' << trip->surcharge << ',' << trip->mta_tax << ','
                << trip->tip_amount << ',' << trip->tolls_amount << ',' << int(trip->passengers);
            for (int field : fields) out << ',' << getExtraFieldValue(trip, field);
            out << '\n';
        }
        out.flush();
        if (out.status()!=QTextStream::Ok) throw std::runtime_error("Could not write CSV");
        cancel.check();
        // Release QObject affinity for a sequential handoff to the GUI thread.
        // A canceled outcome can also safely destroy this detached temporary.
        file->moveToThread(nullptr);
        return file;
    }, [this, filename](std::shared_ptr<QSaveFile> file) {
        // Publish only on the GUI thread after the controller's cancellation check.
        file->moveToThread(thread());
        if (!file->commit()) emit backgroundError(file->errorString());
        else emit exportFinished(filename);
    });
    return true;
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
  if (ui->geographicalView->queryBusy()) return;
  if (this->ui->tabWidget->currentWidget()==this->ui->timeSeriesWidget) {
    int numBins = this->ui->timeSeriesWidget->getNumberOfBins();
    TemporalSeriesDialog *dialog = new TemporalSeriesDialog(this->ui->geographicalView, numBins);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    connect(dialog, &QDialog::accepted, this, [this, dialog] { updateTimes(dialog->startTime(),dialog->endTime()); });
    dialog->open();
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

  if (timeRanges.isEmpty()) return;
  auto graph = std::make_shared<SelectionGraph>();
  graph->assign(ui->geographicalView->getSelectionGraph());
  const auto selection = SelectionSnapshot::capture(graph.get());
  const auto dataset = Global::getInstance()->dataset();
  const auto center = ui->geographicalView->mapView()->center();
  const int zoom = ui->geographicalView->mapView()->zoomLevel();
  auto base = timeRanges.first();
  for (const auto &range : timeRanges)
      if (base.first.secsTo(base.second) < range.first.secsTo(range.second)) base = range;
  if (explorationProgress) explorationProgress->close();
  auto progress = new QProgressDialog(tr("Performing exploration…"), tr("Cancel"), 0, 0, this);
  explorationProgress=progress;
  progress->setAttribute(Qt::WA_DeleteOnClose);
  progress->setWindowModality(Qt::NonModal);
  connect(progress, &QProgressDialog::canceled, this, &ViewWidget::cancelExploration);
  explorationJob.onBusy = [progress=QPointer<QProgressDialog>(progress)](bool busy) { if (!busy && progress) progress->close(); };
  progress->show();
  explorationJob.submit([dataset, selection, timeRanges, base](const Cancellation &cancel) {
      ExplorationData data;
      auto storage = std::make_shared<std::vector<KdTrip::Trip>>();
      for (int i=0; i<timeRanges.size(); ++i) {
          cancel.check();
          const auto range = timeRanges.at(i);
          data.slices.push_back(QueryManager::query(dataset, selection, range.first, range.second, cancel));
          const auto delta = i==0 ? base.second.secsTo(range.second) : base.first.secsTo(range.first);
          size_t n=0;
          for (auto original : data.slices.back()) {
              if ((n++ & 1023)==0) cancel.check();
              auto trip = *original;
              trip.pickup_time -= delta; trip.dropoff_time -= delta;
              trip.pickup_lat = i; trip.pickup_long = i;
              storage->push_back(trip);
          }
      }
      data.plot.keepAlive(storage);
      size_t n=0;
      for (const auto &trip : *storage) {
          if ((n++ & 1023)==0) cancel.check();
          data.plot.insert(&trip);
      }
      return data;
  }, [this, graph, timeRanges, base, center, zoom](ExplorationData data) {
      auto dialog = new TimeExplorationDialog(this);
      dialog->setAttribute(Qt::WA_DeleteOnClose);
      SelectionGraph plotGraph;
      for (int i=0; i<timeRanges.size(); ++i) {
          std::map<int,SelectionGraphNode*>::iterator nb, ne;
          std::map<int,SelectionGraphEdge*>::iterator eb, ee;
          auto group = GroupRepository::getInstance().getItem(i+1);
          graph->getNodeIterator(nb,ne);
          for (;nb!=ne;++nb) nb->second->setGroup(group);
          graph->getEdgeIterator(eb,ee);
          for (;eb!=ee;++eb) eb->second->setGroup(group);
          dialog->addGeoWidget(timeRanges[i].first,timeRanges[i].second,graph.get(),data.slices[i]);
          QPainterPath path; path.addRect(i-.5,i-.5,1,1);
          auto sel = new Selection(path); sel->setType(Selection::START);
          plotGraph.addNode(sel)->setGroup(group);
      }
      dialog->setPlotSelection(base.first,base.second,&plotGraph,&data.plot);
      dialog->setMapView(center,zoom);
      dialog->show();
  });
}
