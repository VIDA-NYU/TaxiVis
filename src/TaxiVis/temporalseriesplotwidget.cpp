#include "temporalseriesplotwidget.h"
#include "ui_temporalseriesplotwidget.h"
#include <iostream>
#include <sstream>
#include "coordinator.h"
#include "global.h"

using namespace std;

TemporalSeriesPlotWidget::TemporalSeriesPlotWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlotWidget),
    selectedTrips(NULL),
    _plotAttribute(TemporalSeriesPlotWidget::NUMBER_OF_TRIPS)
{
    this->setCoordinator(Coordinator::instance());

    ui->setupUi(this);

    //
    setNumBins(ui->numBinsSpin->value());

    //
    Global* global = Global::getInstance();
    int numExtraFields = global->numExtraFields();
    for(int i = 0 ; i < numExtraFields ; ++i){
        ExtraField field = global->getExtraField(i);
        if(field.active){
            ui->attributeComboBox->addItem(field.screenName);
        }
    }

    //
    connect(ui->numBinsSpin,SIGNAL(editingFinished()),this,SLOT(updateNumBins()));

    //
    connect(ui->attributeComboBox,SIGNAL(currentTextChanged(QString)),this,SLOT(updatePlotAttrib(QString)));

    //
    connect(ui->customPlot,SIGNAL(timeIntervalChanged(QDateTime,QDateTime)),this,SLOT(selectionChanged(QDateTime,QDateTime)));

    // connect slots that takes care that when an axis is selected, only that direction can be dragged and zoomed:
    connect(ui->customPlot, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress()));
    connect(ui->customPlot, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel()));

}

TemporalSeriesPlotWidget::~TemporalSeriesPlotWidget()
{
    delete ui;
}

void TemporalSeriesPlotWidget::setSelectedTripsRepository(KdTrip::TripSet *v){
    selectedTrips = v;
}

bool TemporalSeriesPlotWidget::tripSatisfiesEdge(const KdTrip::Trip *trip, SelectionGraphEdge* edge){
    return (edge->getTail()->getSelection()->contains(QPointF(trip->pickup_lat,trip->pickup_long)) &&
            edge->getHead()->getSelection()->contains(QPointF(trip->dropoff_lat,trip->dropoff_long)));
}


bool TemporalSeriesPlotWidget::tripSatisfiesConstraints(const KdTrip::Trip *trip,
                                                        std::vector<SelectionGraphNode*> groupNodeConstraints,
                                                        std::vector<SelectionGraphEdge*> groupEdgeConstraints){
    //this set registers the nodes that are only support for edges
    set<int> alreadyProcessedNodes;
    //test edges
    vector<SelectionGraphEdge*>::iterator edgeIterator;
    for(edgeIterator = groupEdgeConstraints.begin() ; edgeIterator != groupEdgeConstraints.end() ; ++edgeIterator){
        SelectionGraphEdge* edge = *edgeIterator;

        SelectionGraphNode* tail = edge->getTail();
        SelectionGraphNode* head = edge->getHead();

        alreadyProcessedNodes.insert(tail->getId());
        alreadyProcessedNodes.insert(head->getId());

        if(tripSatisfiesEdge(trip, edge))
            return true;
    }

    //test nodes
    vector<SelectionGraphNode*>::iterator nodeIterator;
    for(nodeIterator = groupNodeConstraints.begin() ; nodeIterator != groupNodeConstraints.end() ; ++ nodeIterator){
        SelectionGraphNode* node = *nodeIterator;
        if(alreadyProcessedNodes.count(node->getId()) > 0)
            continue;

        Selection* sel = node->getSelection();

        if((sel->getType() == Selection::START || sel->getType() == Selection::START_AND_END) &&
                sel->contains(QPointF(trip->pickup_lat,trip->pickup_long))){
            return true;
        }
        else if((sel->getType() == Selection::END || sel->getType() == Selection::START_AND_END) &&
                sel->contains(QPointF(trip->dropoff_lat,trip->dropoff_long))){
            return true;
        }
    }

    //
    return false;
}

void TemporalSeriesPlotWidget::setNumBins(int n){
    numBins = n;
}

void TemporalSeriesPlotWidget::recomputePlots(){
    if (suspended || !selectedTrips || !selectionGraph) return;
    const auto trips = *selectedTrips;
    const auto selection = SelectionSnapshot::capture(selectionGraph);
    const auto t0 = startTime.toSecsSinceEpoch(), t1 = endTime.toSecsSinceEpoch();
    const int bins = std::max(1, numBins);
    computeJob.submit([trips, selection, t0, t1, bins](const Cancellation &cancel) {
        std::map<Group, std::vector<HourSlot>> plots;
        const qint64 width = std::max<qint64>(1, (t1-t0)/bins);
        for (const auto &group : selection.groups) {
            auto &timeBins = plots[group.first];
            for (int i=0; i<bins; ++i)
                timeBins.emplace_back(t0+i*width, i==bins-1 ? t1 : t0+(i+1)*width);
        }
        size_t n=0;
        for (const auto *trip : trips) {
            if ((n++ & 1023)==0) cancel.check();
            if (trip->pickup_time < t0 || trip->pickup_time > t1) continue;
            const auto bin = std::min<qint64>(bins-1, (trip->pickup_time-t0)/width);
            const auto last = std::min<qint64>(bins-1, (qint64(trip->dropoff_time)-t0)/width);
            for (auto &group : plots) if (selection.matches(group.first, trip)) {
                group.second[bin].update(trip);
                for (auto b=bin; b<=last; ++b) group.second[b].num_taxis++;
            }
        }
        return plots;
    }, [this](std::map<Group, std::vector<HourSlot>> plots) {
        groupPlots = std::move(plots);
        updatePlots();
        if (Coordinator::instance()->containsTimeSeries(this)) Coordinator::instance()->notifyAll();
    });
}

void TemporalSeriesPlotWidget::setSelectionGraph(SelectionGraph *s){
    selectionGraph = s;
}

void TemporalSeriesPlotWidget::setDateTimes(QDateTime s, QDateTime e){
    //qDebug() << "Set Date Times " << s << "; " << e << endl;
    startTime = s;
    endTime = e;
}

void TemporalSeriesPlotWidget::setCoordinator(Coordinator *c)
{
  this->coordinator = c;
}

void TemporalSeriesPlotWidget::updatePlotAttrib(QString selectedText){

//    qDebug() << "   Update Plot Attrib " << selectedText;

    if(!selectedText.compare(QString::fromLatin1("Num Trips"))){
        _plotAttribute = TemporalSeriesPlotWidget::NUMBER_OF_TRIPS;
    }
    else if(!selectedText.compare(QString::fromLatin1("Fare Amount"))){
        _plotAttribute = TemporalSeriesPlotWidget::FARE_AMOUNT;
    }
    else if(!selectedText.compare(QString::fromLatin1("Tip"))){
        _plotAttribute = TemporalSeriesPlotWidget::TIP_AMOUNT;
    }
    else if(!selectedText.compare(QString::fromLatin1("Total Revenue"))){
        _plotAttribute = TemporalSeriesPlotWidget::TOTAL_AMOUNT;
    }
    else if(!selectedText.compare(QString::fromLatin1("Fare per Trip"))){
        _plotAttribute = TemporalSeriesPlotWidget::FARE_PER_TRIP;
    }
    else if(!selectedText.compare(QString::fromLatin1("Fare per Mile"))){
        _plotAttribute = TemporalSeriesPlotWidget::FARE_PER_MILE;
    }
    else if(!selectedText.compare(QString::fromLatin1("Tips per Trip"))){
        _plotAttribute = TemporalSeriesPlotWidget::TIP_PER_TRIP;
    }
    else if(!selectedText.compare(QString::fromLatin1("Tips per Mile"))){
        _plotAttribute = TemporalSeriesPlotWidget::TIP_PER_MILE;
    }
    else if(!selectedText.compare(QString::fromLatin1("Active Cabs"))){
        _plotAttribute = TemporalSeriesPlotWidget::NUM_TAXIS;
    }
    else if(!selectedText.compare(QString::fromLatin1("Tolls Amount"))){
        _plotAttribute = TemporalSeriesPlotWidget::TOLL_AMOUNT;
    }
    else if(!selectedText.compare(QString::fromLatin1("Tolls Per Trip"))){
        _plotAttribute = TemporalSeriesPlotWidget::TOLL_PER_TRIP;
    }
    else if(!selectedText.compare(QString::fromLatin1("Average Speed"))){
        _plotAttribute = TemporalSeriesPlotWidget::AVG_SPEED_PER_TRIP;
    }
    else if(!selectedText.compare(QString::fromLatin1("Distance"))){
        _plotAttribute = TemporalSeriesPlotWidget::DISTANCE_PER_TRIP;
    }
    else if(!selectedText.compare(QString::fromLatin1("Duration"))){
        _plotAttribute = TemporalSeriesPlotWidget::DURATION_PER_TRIP;
    }
    else{
        //find extra field
        int fieldIndex = Global::getInstance()->getIndexByScreenName(selectedText);
        if(fieldIndex == -1){
            qDebug() << "Invalide Attribute";
            assert(false);
        }
        else{
            switch(fieldIndex){
            case 0:
                _plotAttribute = FIELD1;
                break;
            case 1:
                _plotAttribute = FIELD2;
                break;
            case 2:
                _plotAttribute = FIELD3;
                break;
            case 3:
                _plotAttribute = FIELD4;
                break;
            default:
                assert(false);
                break;
            }
        }
    }

    // update all plots if it is synchronize
    if (Coordinator::instance()->containsTimeSeries(this)) {
        Coordinator::instance()->setSeriesAttribute(_plotAttribute);
        Coordinator::instance()->notifyAll();
    }

    updatePlots();
}

void TemporalSeriesPlotWidget::updateYRange(float min, float max)
{
    //cout << "Update Y range" << endl;
    ui->customPlot->yAxis->setRange(0.95*min, 1.05*max);
    ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
}

void TemporalSeriesPlotWidget::selectionChanged(QDateTime start, QDateTime end){
    emit timeIntervalChanged(start,end);
}

void TemporalSeriesPlotWidget::mousePress(){
    // if an axis is selected, only allow the direction of that axis to be dragged
    // if no axis is selected, both directions may be dragged

    if (ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
        ui->customPlot->axisRect()->setRangeDrag(ui->customPlot->xAxis->orientation());
    else if (ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
        ui->customPlot->axisRect()->setRangeDrag(ui->customPlot->yAxis->orientation());
    else
        ui->customPlot->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
}

void TemporalSeriesPlotWidget::mouseWheel(){
    // if an axis is selected, only allow the direction of that axis to be zoomed
    // if no axis is selected, both directions may be zoomed

    if (ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
        ui->customPlot->axisRect()->setRangeZoom(ui->customPlot->xAxis->orientation());
    else if (ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
        ui->customPlot->axisRect()->setRangeZoom(ui->customPlot->yAxis->orientation());
    else
        ui->customPlot->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
}

void TemporalSeriesPlotWidget::setPlotAttribute(TemporalSeriesPlotWidget::PlotAttribute pAttrib){
    _plotAttribute = pAttrib;

    // DANGEROUS: be carefull the order of combobox items is the same to the enumeration
    // TODO(jpocom): I think I need to disconnec the event and reconnect after the change.
    ui->attributeComboBox->setCurrentIndex(pAttrib);
}

void TemporalSeriesPlotWidget::updateNumBins(){
    setNumBins(ui->numBinsSpin->value());

    recomputePlots();
}

void TemporalSeriesPlotWidget::updatePlots(){
    //ui->customPlot->clearPlottables();
    ui->customPlot->clearGraphs();
    //cout << "UPDATE PLOT" << endl;

    //
    ExtraField field;
    switch(_plotAttribute){
    case(TemporalSeriesPlotWidget::NUMBER_OF_TRIPS):
        ui->customPlot->setTitle("Number of trips per time");
        ui->customPlot->yAxis->setLabel(QString::fromStdString("Num Trips"));
        break;
    case(TemporalSeriesPlotWidget::FARE_AMOUNT):
        ui->customPlot->setTitle("Fare Amount per time");
        ui->customPlot->yAxis->setLabel(QString::fromStdString("US$"));
        break;
    case(TemporalSeriesPlotWidget::TIP_AMOUNT):
        ui->customPlot->setTitle("Tips per time");
        ui->customPlot->yAxis->setLabel(QString::fromStdString("US$"));
        break;
    case(TemporalSeriesPlotWidget::TOTAL_AMOUNT):
        ui->customPlot->setTitle("Total amount per time");
        ui->customPlot->yAxis->setLabel(QString::fromStdString("US$"));
        break;
    case(TemporalSeriesPlotWidget::FARE_PER_TRIP):
        ui->customPlot->setTitle("Fare/trip per time");
        ui->customPlot->yAxis->setLabel(QString::fromStdString("US$/Trip"));
        break;
    case(TemporalSeriesPlotWidget::FARE_PER_MILE):
        ui->customPlot->setTitle("Fare/mile per time");
        ui->customPlot->yAxis->setLabel(QString::fromStdString("US$/Mile"));
        break;
    case(TemporalSeriesPlotWidget::TIP_PER_TRIP):
        ui->customPlot->setTitle("Tips/trip per time");
        ui->customPlot->yAxis->setLabel(QString::fromStdString("US$/Trip"));
        break;
    case(TemporalSeriesPlotWidget::TIP_PER_MILE):
        ui->customPlot->setTitle("Tips/mile per time");
        ui->customPlot->yAxis->setLabel(QString::fromStdString("US$/Mile"));
        break;
    case(TemporalSeriesPlotWidget::NUM_TAXIS):
        ui->customPlot->setTitle("Active cabs per time");
        ui->customPlot->yAxis->setLabel(QString::fromStdString("cab"));
        break;
    case(TemporalSeriesPlotWidget::TOLL_AMOUNT):
        ui->customPlot->setTitle("Tolls Amount per time");
        ui->customPlot->yAxis->setLabel(QString::fromStdString("tools ($)"));
        break;
    case(TemporalSeriesPlotWidget::TOLL_PER_TRIP):
        ui->customPlot->setTitle("Tolls Amount per trip");
        ui->customPlot->yAxis->setLabel(QString::fromStdString("tools ($)"));
        break;
    case(TemporalSeriesPlotWidget::DISTANCE_PER_TRIP):
        ui->customPlot->setTitle("Distance Per Trip per time");
        ui->customPlot->yAxis->setLabel(QString::fromStdString("Distance in Miles"));
        break;
    case(TemporalSeriesPlotWidget::DURATION_PER_TRIP):
        ui->customPlot->setTitle("Duration Per Trip per time");
        ui->customPlot->yAxis->setLabel(QString::fromStdString("Duration in minutes"));
        break;
    case(TemporalSeriesPlotWidget::AVG_SPEED_PER_TRIP):
        ui->customPlot->setTitle("Average Speed per time");
        ui->customPlot->yAxis->setLabel(QString::fromStdString("Avg Speed in Miles/hour"));
        break;
    //
    case(FIELD1):
        field = Global::getInstance()->getExtraField(0);
        ui->customPlot->setTitle(field.screenName + "/trip per time");
        ui->customPlot->yAxis->setLabel(field.axisLabel);
        break;
    case(FIELD2):
        field = Global::getInstance()->getExtraField(1);
        ui->customPlot->setTitle(field.screenName + "/trip per time");
        ui->customPlot->yAxis->setLabel(field.axisLabel);
        break;
    case(FIELD3):
        field = Global::getInstance()->getExtraField(2);
        ui->customPlot->setTitle(field.screenName + "/trip per time");
        ui->customPlot->yAxis->setLabel(field.axisLabel);
        break;
    case(FIELD4):
        field = Global::getInstance()->getExtraField(3);
        ui->customPlot->setTitle(field.screenName + "/trip per time");
        ui->customPlot->yAxis->setLabel(field.axisLabel);
        break;
    default:
        cout << "ERROR: Invalid plot attribute1" << endl;
        //        exit(1);
    }

    _yMin = std::numeric_limits<float>::max();
    _yMax = std::numeric_limits<float>::lowest();
    for (const auto &entry : groupPlots) {
        Group group = entry.first;
        const auto &plot = entry.second;
        int plotSize = plot.size();
        QVector<double> x(plotSize), y(plotSize);
        for (int j = 0; j < plotSize; ++j) x[j] = plot[j].startTime;
        QCPGraph *graph = ui->customPlot->addGraph();
        graph->setPen(QPen(group.getColor()));
        for(int j = 0 ; j < plotSize ; ++j){
            HourSlot hSlot = plot.at(j);

            float normalizationFactorTrips = (hSlot.num_trips == 0)?1:hSlot.num_trips;
            float normalizationFactorMiles = (hSlot.sum_distance == 0)?1:hSlot.sum_distance;

            switch(_plotAttribute){
            case(TemporalSeriesPlotWidget::NUMBER_OF_TRIPS):
                y[j] = hSlot.num_trips;
                break;
            case(TemporalSeriesPlotWidget::FARE_AMOUNT):
                y[j] = hSlot.sum_fare_amount/100.0;
                break;
            case(TemporalSeriesPlotWidget::TIP_AMOUNT):
                y[j] = hSlot.sum_tips/100.0;
                break;
            case(TemporalSeriesPlotWidget::TOTAL_AMOUNT):
                y[j] = (hSlot.sum_tips + hSlot.sum_fare_amount)/100.0;
                break;
            case(TemporalSeriesPlotWidget::FARE_PER_TRIP):
                y[j] = (hSlot.sum_fare_amount/(100.0*normalizationFactorTrips));
                break;
            case(TemporalSeriesPlotWidget::FARE_PER_MILE):
                y[j] = (hSlot.sum_fare_amount/(100.0*normalizationFactorMiles));
                break;
            case(TemporalSeriesPlotWidget::TIP_PER_TRIP):
                y[j] = (hSlot.sum_tips/(100.0*normalizationFactorTrips));
                break;
            case(TemporalSeriesPlotWidget::TIP_PER_MILE):
                y[j] = (hSlot.sum_tips/(100.0*normalizationFactorMiles));
                break;
            case(TemporalSeriesPlotWidget::NUM_TAXIS):
                y[j] = hSlot.num_taxis;
                break;
            case(TemporalSeriesPlotWidget::TOLL_AMOUNT):
                y[j] = (hSlot.sum_tools_amount/100.0);
                break;
            case(TemporalSeriesPlotWidget::TOLL_PER_TRIP):
                y[j] = (hSlot.sum_tools_amount/(100.0*normalizationFactorTrips));
                break;
            case(TemporalSeriesPlotWidget::DURATION_PER_TRIP):
                y[j] = (hSlot.sum_duration)/(60*normalizationFactorTrips);
                break;
            case(TemporalSeriesPlotWidget::DISTANCE_PER_TRIP):
                y[j] = (hSlot.sum_distance)/(normalizationFactorTrips*100.0);
                break;
            case(TemporalSeriesPlotWidget::AVG_SPEED_PER_TRIP):
                y[j] = ((hSlot.sum_avg_speed*36)/(normalizationFactorTrips));
                break;
            case(TemporalSeriesPlotWidget::FIELD1):
                y[j] = ((hSlot.sum_field1)/(normalizationFactorTrips));
                break;
            case(TemporalSeriesPlotWidget::FIELD2):
                y[j] = ((hSlot.sum_field2)/(normalizationFactorTrips));
                break;
            case(TemporalSeriesPlotWidget::FIELD3):
                y[j] = ((hSlot.sum_field3)/(normalizationFactorTrips));
                break;
            case(TemporalSeriesPlotWidget::FIELD4):
                y[j] = ((hSlot.sum_field4)/(normalizationFactorTrips));
                break;
            default:
                cout << "ERROR: Invalid plot attribute 2" << endl;
            }

            if(y[j] < _yMin)
                _yMin = y[j];
            if(y[j] > _yMax)
                _yMax = y[j];
        }
        graph->setData(x, y);
    }
    ui->customPlot->rescaleAxes();
    if (_yMin <= _yMax) ui->customPlot->yAxis->setRange(0.95*_yMin, 1.05*_yMax);
    QSharedPointer<QCPAxisTickerDateTime> ticker(new QCPAxisTickerDateTime);
    ticker->setDateTimeFormat("hh:mm\nMMM dd");
    ticker->setDateTimeSpec(Qt::LocalTime);
    ui->customPlot->xAxis->setTicker(ticker);
    ui->customPlot->xAxis->setTickLabelFont(QFont(QFont().family(), 10));
    ui->customPlot->yAxis->setTickLabelFont(QFont(QFont().family(), 10));
    ui->customPlot->axisRect()->setRangeDrag(Qt::Horizontal | Qt::Vertical);
    ui->customPlot->axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);
    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
                                    QCP::iSelectLegend | QCP::iSelectPlottables | QCP::iSelectOther);
    ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
}

void TemporalSeriesPlotWidget::setNumberOfBins(int n)
{
    this->ui->numBinsSpin->setValue(n);
    this->updateNumBins();
}

int TemporalSeriesPlotWidget::getNumberOfBins()
{
    return this->ui->numBinsSpin->value();
}
