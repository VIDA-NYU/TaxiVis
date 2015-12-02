#include "temporalseriesplotwidget.h"
#include "ui_temporalseriesplotwidget.h"
#include <iostream>
#include <sstream>
#include "coordinator.h"
#include "global.h"

using namespace std;

QString tmToString(time_t t){
    struct tm* st_tm =  localtime (&t);
    string monthName;
    switch(st_tm->tm_mon){
    case(0):
        monthName = "Jan";
        break;
    case(1):
        monthName = "Feb";
        break;
    case(2):
        monthName = "Mar";
        break;
    case(3):
        monthName = "Apr";
        break;
    case(4):
        monthName = "May";
        break;
    case(5):
        monthName = "Jun";
        break;
    case(6):
        monthName = "Jul";
        break;
    case(7):
        monthName = "Aug";
        break;
    case(8):
        monthName = "Sep";
        break;
    case(9):
        monthName = "Oct";
        break;
    case(10):
        monthName = "Nov";
        break;
    case(11):
        monthName = "Dec";
        break;
    }

    char buff[1000];
    return QString(buff);
}

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
    connect(ui->attributeComboBox,SIGNAL(currentIndexChanged(QString)),this,SLOT(updatePlotAttrib(QString)));

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

void TemporalSeriesPlotWidget::computePlots(){

    if(selectionGraph == NULL)
        return;

    //
    QDate startDate = startTime.date();
    QTime startHour = startTime.time();
    QDate endDate   = endTime.date();
    QTime endHour   = endTime.time();

    //qDebug() << "               Start DateTime " << startTime << " End DateTime " << endTime;

    //
    groupPlots.clear();

    uint64_t t0 = KdTrip::Query::createTime(startDate.year(),startDate.month(),startDate.day(),startHour.hour(),startHour.minute(),startHour.second());
    uint64_t t1 = KdTrip::Query::createTime(endDate.year(),endDate.month(),endDate.day(),endHour.hour(),endHour.minute(),endHour.second());
    unsigned numberOfBins = numBins;
    int binSizeInSeconds = (t1-t0)/(numberOfBins);
    //    qDebug() << "StartTime " <<startTime << " endTime " << endTime;
    //    qDebug() << "Size in seconds " << binSizeInSeconds;
    //assert(binSizeInSeconds == ((t1-t0)/numberOfBins));
    bool buildGlobalPlot = (selectionGraph->isEmpty());

    //initialize group plots
    vector<HourSlot> templatePlot;
    for(unsigned i = 0 ; i < numberOfBins ; ++i){
        uint64_t slotStart = t0 + (binSizeInSeconds * i);
        uint64_t slotEnd;
        if(i == numberOfBins - 1)
            slotEnd   = t1;
        else
            slotEnd   = t0 + (binSizeInSeconds * (i+1));

        HourSlot currentSlot(slotStart,slotEnd);
        templatePlot.push_back(currentSlot);
    }

    set<Group> groups;
    map<Group,vector<SelectionGraphNode*> > mapGroupToNodes;
    map<Group,vector<SelectionGraphEdge*> > mapGroupToEdges;
    selectionGraph->groupNodesAndEdgeByColor(groups,mapGroupToNodes,mapGroupToEdges);
    set<Group>::iterator groupIterator;


    //
    set<Group> notEmptyGroups;
    map<Group,vector<SelectionGraphNode*> > tempMapGroupToNodes;

    //
    if(buildGlobalPlot){
        groupPlots[Group(Qt::black)] = templatePlot;
    }
    else{
        for(groupIterator = groups.begin() ; groupIterator != groups.end() ; ++groupIterator){
            vector<SelectionGraphNode*> &groupNodes = mapGroupToNodes[*groupIterator];
            vector<SelectionGraphNode*> validGroupNodes;
            int numGroupNodes = groupNodes.size();

            for(int i = 0 ; i < numGroupNodes ; ++i){
                SelectionGraphNode* node = groupNodes.at(i);
                if(node->inDegree() + node->outDegree() == 0)
                    validGroupNodes.push_back(node);
            }

            vector<SelectionGraphEdge*> &groupEdges = mapGroupToEdges[*groupIterator];
            if(groupEdges.size() + validGroupNodes.size() > 0){
                notEmptyGroups.insert(*groupIterator);
                tempMapGroupToNodes[*groupIterator] = validGroupNodes;
            }
        }

        //
        groups.clear();
        groups = notEmptyGroups;
        mapGroupToNodes.clear();
        mapGroupToNodes = tempMapGroupToNodes;

        //
        for(groupIterator = groups.begin() ; groupIterator != groups.end() ; ++groupIterator){
            groupPlots[*groupIterator] = templatePlot;
        }
    }

    //
    KdTrip::TripSet::iterator it;

    for(it = selectedTrips->begin() ; it != selectedTrips->end() ; ++it){

        const KdTrip::Trip *trip = *it;
        uint64_t bin = (trip->pickup_time - t0)/binSizeInSeconds;
        if(bin == numberOfBins){//put the trips starting at the final time in the last bin
            --bin;
        }

        //
        if(buildGlobalPlot){
            vector<HourSlot> &groupPlot  = groupPlots[Group(Qt::black)];
            HourSlot &currentSlot        = groupPlot.at(bin);
            currentSlot.update(trip);
//            currentSlot.num_trips        += 1;
//            currentSlot.sum_distance     += trip->distance;
//            currentSlot.sum_fare_amount  += trip->fare_amount;
//            currentSlot.sum_tips         += trip->tip_amount;
//            currentSlot.sum_tools_amount += trip->tolls_amount/100.f;
//            //
//            currentSlot.sum_field1       += trip->field1;
//            currentSlot.sum_field2       += trip->field2;
//            currentSlot.sum_field3       += trip->field3;
//            currentSlot.sum_field4       += trip->field4;

//            int32_t tripDuration = (trip->dropoff_time- trip->pickup_time);
//            if(tripDuration > 0){
//                currentSlot.sum_duration  += tripDuration;
//                currentSlot.sum_avg_speed += (trip->distance/(tripDuration));
//            }

            uint64_t binEnd = (trip->dropoff_time-t0)/binSizeInSeconds;
            if(binEnd>=numberOfBins)
                binEnd = numberOfBins-1;
            for (uint64_t b=bin; b<=binEnd; ++b)
                groupPlot.at(b).num_taxis++;
        }
        else{
            for(groupIterator = groups.begin() ; groupIterator != groups.end() ; ++groupIterator){
                Group currentGroup = *groupIterator;

                assert(mapGroupToNodes.count(currentGroup) > 0 && mapGroupToEdges.count(currentGroup) > 0);

                if(tripSatisfiesConstraints(trip, mapGroupToNodes[currentGroup],mapGroupToEdges[currentGroup])){
                    vector<HourSlot> &groupPlot  = groupPlots[currentGroup];
                    HourSlot &currentSlot        = groupPlot.at(bin);
                    currentSlot.update(trip);
//                    currentSlot.num_trips       += 1;
//                    currentSlot.sum_distance    += trip->distance/100.f;
//                    currentSlot.sum_fare_amount += trip->fare_amount/100.f;
//                    currentSlot.sum_tips        += trip->tip_amount/100.f;
//                    currentSlot.sum_tools_amount += trip->tolls_amount/100.f;
//
//                    int32_t tripDuration = (trip->dropoff_time- trip->pickup_time);
//                    if(tripDuration > 0){
//                        currentSlot.sum_duration  += tripDuration;
//                        currentSlot.sum_avg_speed += (trip->distance/(tripDuration));
//                    }
                }
            }
        }
    }
}

void TemporalSeriesPlotWidget::setNumBins(int n){
    numBins = n;
}

void TemporalSeriesPlotWidget::recomputePlots(){

    //qDebug() << "   TemporalTimeSeries recomputePlots()";

    //
    computePlots();
    //qDebug() << "        after computePlots" << endl;

    //
    updatePlots();
    //qDebug() << "        after updatePlots" << endl;

    // update all plots if it is synchronize
    if (Coordinator::instance()->containsTimeSeries(this)){
        Coordinator::instance()->notifyAll();

        //qDebug() << "        after notifyAll" << endl;
    }
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

    if(!selectedText.compare(QString::fromAscii("Num Trips"))){
        _plotAttribute = TemporalSeriesPlotWidget::NUMBER_OF_TRIPS;
    }
    else if(!selectedText.compare(QString::fromAscii("Fare Amount"))){
        _plotAttribute = TemporalSeriesPlotWidget::FARE_AMOUNT;
    }
    else if(!selectedText.compare(QString::fromAscii("Tip"))){
        _plotAttribute = TemporalSeriesPlotWidget::TIP_AMOUNT;
    }
    else if(!selectedText.compare(QString::fromAscii("Total Revenue"))){
        _plotAttribute = TemporalSeriesPlotWidget::TOTAL_AMOUNT;
    }
    else if(!selectedText.compare(QString::fromAscii("Fare per Trip"))){
        _plotAttribute = TemporalSeriesPlotWidget::FARE_PER_TRIP;
    }
    else if(!selectedText.compare(QString::fromAscii("Fare per Mile"))){
        _plotAttribute = TemporalSeriesPlotWidget::FARE_PER_MILE;
    }
    else if(!selectedText.compare(QString::fromAscii("Tips per Trip"))){
        _plotAttribute = TemporalSeriesPlotWidget::TIP_PER_TRIP;
    }
    else if(!selectedText.compare(QString::fromAscii("Tips per Mile"))){
        _plotAttribute = TemporalSeriesPlotWidget::TIP_PER_MILE;
    }
    else if(!selectedText.compare(QString::fromAscii("Active Cabs"))){
        _plotAttribute = TemporalSeriesPlotWidget::NUM_TAXIS;
    }
    else if(!selectedText.compare(QString::fromAscii("Tolls Amount"))){
        _plotAttribute = TemporalSeriesPlotWidget::TOLL_AMOUNT;
    }
    else if(!selectedText.compare(QString::fromAscii("Tolls Per Trip"))){
        _plotAttribute = TemporalSeriesPlotWidget::TOLL_PER_TRIP;
    }
    else if(!selectedText.compare(QString::fromAscii("Average Speed"))){
        _plotAttribute = TemporalSeriesPlotWidget::AVG_SPEED_PER_TRIP;
    }
    else if(!selectedText.compare(QString::fromAscii("Distance"))){
        _plotAttribute = TemporalSeriesPlotWidget::DISTANCE_PER_TRIP;
    }
    else if(!selectedText.compare(QString::fromAscii("Duration"))){
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
    ui->customPlot->replot();
}

void TemporalSeriesPlotWidget::selectionChanged(QDateTime start, QDateTime end){
    emit timeIntervalChanged(start,end);
}

void TemporalSeriesPlotWidget::mousePress(){
    // if an axis is selected, only allow the direction of that axis to be dragged
    // if no axis is selected, both directions may be dragged

    if (ui->customPlot->xAxis->selected().testFlag(QCPAxis::spAxis))
        ui->customPlot->setRangeDrag(ui->customPlot->xAxis->orientation());
    else if (ui->customPlot->yAxis->selected().testFlag(QCPAxis::spAxis))
        ui->customPlot->setRangeDrag(ui->customPlot->yAxis->orientation());
    else
        ui->customPlot->setRangeDrag(Qt::Horizontal|Qt::Vertical);
}

void TemporalSeriesPlotWidget::mouseWheel(){
    // if an axis is selected, only allow the direction of that axis to be zoomed
    // if no axis is selected, both directions may be zoomed

    if (ui->customPlot->xAxis->selected().testFlag(QCPAxis::spAxis))
        ui->customPlot->setRangeZoom(ui->customPlot->xAxis->orientation());
    else if (ui->customPlot->yAxis->selected().testFlag(QCPAxis::spAxis))
        ui->customPlot->setRangeZoom(ui->customPlot->yAxis->orientation());
    else
        ui->customPlot->setRangeZoom(Qt::Horizontal|Qt::Vertical);
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

    //
    map<Group, std::vector<HourSlot> >::iterator it;

    bool first = true;

    QVector<double> x;
    QVector<double> ticks;
    QVector<QString> labels;

    int plotIndex = 0;
    int plotSize = 0;


    for(it = groupPlots.begin() ; it != groupPlots.end() ; ++it){
        Group group            = it->first;
        vector<HourSlot>& plot = it->second;
        plotSize = plot.size();

        if(first){
            //domain
            x.resize(plotSize);

            for(int j = 0 ; j < plotSize ; ++j){
                HourSlot hs = plot.at(j);
                x[j] = hs.startTime;//Make local time//j;
                ticks << j;
                //TODO: DON't do this conversion here
                time_t t = hs.startTime;

                //
                labels << (tmToString(t));
            }

            first = false;
        }

        //
        ui->customPlot->addGraph();
        ui->customPlot->graph(plotIndex)->setPen(QPen(group.getColor()));
        QVector<double> y(plotSize);

        ui->customPlot->clearPlottables();
        //
        map<Group, std::vector<HourSlot> >::iterator it;

        bool first = true;

        QVector<double> x;
        QVector<double> ticks;
        QVector<QString> labels;

        int plotIndex = 0;
        int plotSize = 0;

        _yMin =  1e10;
        _yMax = -1e10;
        for(it = groupPlots.begin() ; it != groupPlots.end() ; ++it){
            Group group            = it->first;
            vector<HourSlot>& plot = it->second;
            plotSize = plot.size();

            if(first){
                //domain
                x.resize(plotSize);

                for(int j = 0 ; j < plotSize ; ++j){
                    HourSlot hs = plot.at(j);
                    x[j] = hs.startTime;//Make local time//j;
                    ticks << j;
                    //TODO: DON't do this conversion here
                    time_t t = hs.startTime;
                    labels << (tmToString(t));//hs.startTime.toString();//QString::fromStdString(nytaxi::timestampAsString(hs.hour).substr(11));
                }

                first = false;
            }

            //
            ui->customPlot->addGraph();
            ui->customPlot->graph(plotIndex)->setPen(QPen(group.getColor()));
            QVector<double> y(plotSize);

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
            ui->customPlot->graph(0)->rescaleAxes(true);
            ui->customPlot->graph(plotIndex)->setData(x, y);
            ++plotIndex;
        }
        //        int binSize = x[1] - x[0];

        // configure right and top axis to show ticks but no labels (could've also just called ui->customPlot->setupFullAxesBox):
        ui->customPlot->graph(0)->rescaleAxes();
        ui->customPlot->xAxis->setTickVector(ticks);
        ui->customPlot->xAxis->setTickVectorLabels(labels);

        //
        ui->customPlot->yAxis->setRange(0.95*_yMin, 1.05*_yMax);

        //
        // configure bottom axis to show date and time instead of number:
        ui->customPlot->xAxis->setTickLabelType(QCPAxis::ltDateTime);
        //set label format
        ui->customPlot->xAxis->setDateTimeFormat("hh:mm\nMMM dd");
        //        switch(binSizeUnit){
        //        case(TemporalSeriesPlotWidget::MINUTE):
        //            ui->customPlot->xAxis->setDateTimeFormat("hh:mm\nMMM dd");
        //            break;
        //        case(TemporalSeriesPlotWidget::HOUR):
        //            ui->customPlot->xAxis->setDateTimeFormat("hh:mm\nMMM dd");
        //            break;
        //        case(TemporalSeriesPlotWidget::DAY):
        //            //ui->customPlot->xAxis->setDateTimeFormat("dd.MM.yy");
        //            break;
        //        case(TemporalSeriesPlotWidget::MONTH):
        //            ui->customPlot->xAxis->setDateTimeFormat("MMM");
        //            break;
        //        }

        // set a more compact font size for bottom and left axis tick labels:
        ui->customPlot->xAxis->setTickLabelFont(QFont(QFont().family(), 10));
        ui->customPlot->yAxis->setTickLabelFont(QFont(QFont().family(), 10));
        // set a fixed tick-step to one tick per month:
        ui->customPlot->xAxis->setAutoTickStep(true);
        //ui->customPlot->xAxis->setTickStep(binSize); // one month in seconds

        ui->customPlot->setRangeDrag(Qt::Horizontal | Qt::Vertical);
        ui->customPlot->setRangeZoom(Qt::Horizontal | Qt::Vertical);
        ui->customPlot->setInteraction(QCustomPlot::iSelectPlottables); // allow selection of graphs via mouse click

        ui->customPlot->setInteractions(QCustomPlot::iRangeDrag | QCustomPlot::iRangeZoom | QCustomPlot::iSelectAxes |
                                        QCustomPlot::iSelectLegend | QCustomPlot::iSelectPlottables | QCustomPlot::iSelectTitle);

        ui->customPlot->replot();
    }

    ui->customPlot->replot();
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
