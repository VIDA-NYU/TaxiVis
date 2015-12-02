#include "histogramwidget.h"
#include "ui_histogramwidget.h"
#include "coordinator.h"

#include <iostream>
#include <sstream>
#include "global.h"
#include "UsefulGeometryFunctions.h"

using namespace std;

HistogramWidget::HistogramWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HistogramWidget),
    _plotAttribute(HistogramWidget::FARE_AMOUNT)
{
    coordinator = Coordinator::instance();
    ui->setupUi(this);
    ui->histogram->xAxis->setTickLabelFont(QFont("Arial", 11));
    ui->histogram->yAxis->setTickLabelFont(QFont("Arial", 11));

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
    setNumberOfBins(ui->numBinsSpinBox->value());
    connect(ui->numBinsSpinBox,SIGNAL(editingFinished()),this,SLOT(updateNumBins()));

    //
    connect(ui->attributeComboBox,SIGNAL(currentIndexChanged(QString)),this,SLOT(changeHistAttribute(QString)));

    //
    // connect slots that takes care that when an axis is selected, only that direction can be dragged and zoomed:
    connect(ui->histogram, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress(QMouseEvent*)));
    connect(ui->histogram, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel()));
    connect(ui->histogram, SIGNAL(mouseDoubleClick(QMouseEvent*)), this, SLOT(mouseDouble(QMouseEvent*)));
}

HistogramWidget::~HistogramWidget()
{
    delete ui;
}

void HistogramWidget::setSelectedTripsRepository(KdTrip::TripSet *v){
    selectedTrips = v;
}

void HistogramWidget::setSelectionGraph(SelectionGraph *v){
    this->selectionGraph = v;
}

void HistogramWidget::computeDataBounds(){

    //
    for(int i = int(HistogramWidget::FARE_AMOUNT) ; i != int(HistogramWidget::FIELD4) ; i++)
        histogramDataBounds[HistogramWidget::PlotAttribute(i)] = make_pair(numeric_limits<float>::max(),numeric_limits<float>::min());

    //
    KdTrip::TripSet::iterator it;
    for(it = selectedTrips->begin() ; it != selectedTrips->end() ; ++it){
        const KdTrip::Trip *trip = *it;

        // only use valid trips
        if(trip->dropoff_time < trip->pickup_time)
            continue;

        for(int i = int(HistogramWidget::FARE_AMOUNT) ; i != int(HistogramWidget::FIELD4) ; i++){
            pair<float,float>& bounds = histogramDataBounds[(HistogramWidget::PlotAttribute)i];
            float tripAttValue = getTripValue(trip,(HistogramWidget::PlotAttribute)i);
            updateBounds(bounds,tripAttValue);
        }
    }

    //
    map<PlotAttribute, std::pair<float,float> >::iterator bit = histogramDataBounds.begin();
    for(; bit != histogramDataBounds.end() ; ++bit){
        pair<float,float> &bounds = bit->second;
        if(bounds.first == bounds.second){
            bit->second = make_pair(bounds.first - 1,bounds.first + 1);
        }
    }

    //        cout << "Data Bounds" << endl;
    //        pair<float,float> bounds = histogramDataBounds[HistogramWidget::FARE_AMOUNT];
    //        cout << "   Fare Bounds " << bounds.first << " " << bounds.second << endl;
    //        bounds = histogramDataBounds[HistogramWidget::TIP_AMOUNT];
    //        cout << "   Tip Bounds " << bounds.first << " " << bounds.second << endl;
    //        bounds = histogramDataBounds[HistogramWidget::DISTANCE];
    //        cout << "   Distance Bounds " << bounds.first << " " << bounds.second << endl;
    //        bounds = histogramDataBounds[HistogramWidget::DURATION];
    //        cout << "   Duration Bounds " << bounds.first << " " << bounds.second << endl;
}

bool HistogramWidget::tripSatisfiesEdge(const KdTrip::Trip *trip, SelectionGraphEdge* edge){
    return (edge->getTail()->getSelection()->contains(QPointF(trip->pickup_lat,trip->pickup_long)) &&
            edge->getHead()->getSelection()->contains(QPointF(trip->dropoff_lat,trip->dropoff_long)));
}

bool HistogramWidget::tripSatisfiesConstraints(const KdTrip::Trip *trip,
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

        if(tripSatisfiesEdge(trip,edge))
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

void HistogramWidget::updateControlValues(){
    if(groupPlots.size() == 0){
        cerr << "grouop plots 0" << endl;
        return;
    }

    //get reference values
    pair<float,float> attribBounds = histogramDataBounds[_plotAttribute];


    QCPBars* b = groupPlots.begin()->second;
    float barWidth = b->width();

    const QCPBarDataMap* datamap = b->data();
    QCPBarDataMap::const_iterator it = datamap->begin();

    double key = it.key();
    double minBin = key - barWidth/2;


    //
    QMapIterator<double, QCPBarData> i(*datamap);
    i.toBack();
    i.previous();
    key = i.key();
    double maxBin = key + barWidth/2;

    //
    ui->histogram->setControlValues(minBin,attribBounds.first,maxBin,attribBounds.second);

//    cout << "Update Control Values: x0 = " <<
    //            minBin << " y0 = " << attribBounds.first << " x1 = " << maxBin << " y1 = " << attribBounds.second << endl;
}

float HistogramWidget::getTripValue(const KdTrip::Trip *trip,HistogramWidget::PlotAttribute attrib){
    float tripAttribValue = -1;
    switch(attrib){
    case(FARE_AMOUNT):
        //cout << "Fare amount " << endl;
        tripAttribValue = trip->fare_amount/100.0;
        break;
    case(TIP_AMOUNT):
        //cout << "Tip amount " << endl;
        tripAttribValue = trip->tip_amount/100.0;
        break;
    case(DISTANCE):
        //cout << "Distance " << endl;
        tripAttribValue = trip->distance/100.0;
        break;
    case(DURATION):
        tripAttribValue = ((trip->dropoff_time - trip->pickup_time)/60);
        //cout << "Duration " << (trip->dropoff_time - trip->pickup_time) << endl;
        break;
    case(FIELD1):
        tripAttribValue = trip->field1;
        //cout << "Field 1 " << trip->field1 << endl;
        break;
    case(FIELD2):
        tripAttribValue = trip->field2;
        //cout << "Field 1 " << trip->field2 << endl;
        break;
    case(FIELD3):
        tripAttribValue = trip->field3;
        //cout << "Field 1 " << trip->field3 << endl;
        break;
    case(FIELD4):
        tripAttribValue = trip->field4;
        //cout << "Field 1 " << trip->field4 << endl;
        break;
    default:
        exit(9);
        break;
    }
    return tripAttribValue;
}

void HistogramWidget::computeHistograms() {
    if(selectionGraph == NULL)
        return;

    //
    groupHistograms.clear();
    histogramDataBounds.clear();

    //compute bounds
    computeDataBounds();

    // compute bins
    std::map<PlotAttribute, std::vector<HistBin> > templateHistograms;
    map<PlotAttribute, std::pair<float,float> >::iterator boundsIterator;
    for(boundsIterator = histogramDataBounds.begin();boundsIterator != histogramDataBounds.end() ; ++boundsIterator){
        PlotAttribute attribute = boundsIterator->first;
        pair<float,float> bounds = boundsIterator->second;
        float binSize = (bounds.second - bounds.first)/numberOfBins;

        vector<HistBin>plot;

        for(int i = 0 ; i < numberOfBins ; ++i){
            float minBin = bounds.first + i*binSize;
            float maxBin = minBin + binSize;
            HistBin bin;
            bin.minBin = minBin;
            bin.maxBin = maxBin;
            bin.maxBin = 0.0f;

            plot.push_back(bin);
        }

        templateHistograms[attribute] = plot;
    }
    //initialize histograms
    bool buildGlobalPlot = (selectionGraph->isEmpty());
    set<Group> groups;
    map<Group,vector<SelectionGraphNode*> > mapGroupToNodes;
    map<Group,vector<SelectionGraphEdge*> > mapGroupToEdges;
    selectionGraph->groupNodesAndEdgeByColor(groups,mapGroupToNodes,mapGroupToEdges);
    set<Group>::iterator groupIterator;

    //
    set<Group> notEmptyGroups;
    map<Group,vector<SelectionGraphNode*> > tempMapGroupToNodes;

    if(buildGlobalPlot){
        groupHistograms[Group(Qt::black)] = templateHistograms;
    }
    else{
        //
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
            groupHistograms[*groupIterator] = templateHistograms;
        }
    }

    //
    KdTrip::TripSet::iterator it;

    for(it = selectedTrips->begin() ; it != selectedTrips->end() ; ++it){
        const KdTrip::Trip *trip = *it;

        // only use valid trips
        if(trip->dropoff_time < trip->pickup_time)
            continue;

        if(buildGlobalPlot){
            map<PlotAttribute, vector<HistBin> > &groupHists  = groupHistograms[Group(Qt::black)];
            map<PlotAttribute, vector<HistBin> >::iterator attIterator;
            for(attIterator = groupHists.begin() ; attIterator != groupHists.end() ; ++attIterator){
                PlotAttribute    currentAttb = attIterator->first;
                vector<HistBin> &currentHist = attIterator->second;

                pair<float,float> attribBounds = histogramDataBounds[currentAttb];
                float binSize = (attribBounds.second - attribBounds.first)/numberOfBins;
                if (binSize<1e-6) continue;

                float tripAttribValue = getTripValue(trip,currentAttb);

                int binIndex = (tripAttribValue - attribBounds.first)/binSize;
                if(binIndex == numberOfBins)
                    --binIndex;

                if(currentHist.size() <= binIndex){
                    cout << "Trip.fareamount " << trip->fare_amount << endl;
                    cout << "Trip Attrib Value " << tripAttribValue << " attribBounds.first " << attribBounds.first << endl;
                    cout << "attribBounds.first " << attribBounds.first << " attribBounds.second " << attribBounds.second << endl;
                    cout << "Current Hist Size " << currentHist.size() << " binIndex " << binIndex << endl;
                    exit(1);
                }
                HistBin& bin = currentHist[binIndex];
                bin.freq += 1;
            }
        }
        else{
            for(groupIterator = groups.begin() ; groupIterator != groups.end() ; ++groupIterator){
                Group currentGroup = *groupIterator;

                assert(mapGroupToNodes.count(currentGroup) > 0 && mapGroupToEdges.count(currentGroup) > 0);

                if(tripSatisfiesConstraints(trip, mapGroupToNodes[currentGroup],mapGroupToEdges[currentGroup])){
                    //update group hist
                    map<PlotAttribute, vector<HistBin> > &groupHists  = groupHistograms[currentGroup];
                    map<PlotAttribute, vector<HistBin> >::iterator attIterator;
                    for(attIterator = groupHists.begin() ; attIterator != groupHists.end() ; ++attIterator){
                        PlotAttribute    currentAttb = attIterator->first;
                        vector<HistBin> &currentHist = attIterator->second;

                        pair<float,float> attribBounds = histogramDataBounds[currentAttb];
                        float binSize = (attribBounds.second - attribBounds.first)/numberOfBins;
                        if (binSize<1e-6) continue;
                        float tripAttribValue = getTripValue(trip,currentAttb);

                        int binIndex = (tripAttribValue - attribBounds.first)/binSize;
                        if(binIndex == numberOfBins)
                            --binIndex;

                        if(!(0 <= binIndex && binIndex < currentHist.size())){
                            cout << "Bin index " << binIndex << " histSize " << currentHist.size() << " numberOfBins " << numberOfBins << " tripValue " << tripAttribValue << " attribBounds.first " << attribBounds.first << endl;
                            cout << "    intended bin " << (tripAttribValue - attribBounds.first)/binSize << endl;
                            assert(0 <= binIndex && binIndex < currentHist.size());
                        }

                        HistBin& bin = currentHist[binIndex];
                        bin.freq += 1;
                    }


                }
            }
        }
    }
}

void HistogramWidget::updatePlots(){

    //
    updateControlValues();

    //
    ui->histogram->clearPlottables();

    //
    ExtraField field;
    switch(_plotAttribute){
    case(HistogramWidget::FARE_AMOUNT):
        ui->histogram->setTitle("Fare Amount");
        ui->histogram->xAxis->setLabel(QString::fromStdString("US$"));
        break;
    case(HistogramWidget::TIP_AMOUNT):
        ui->histogram->setTitle("Tip Amount");
        ui->histogram->xAxis->setLabel(QString::fromStdString("US$"));
        break;
    case(HistogramWidget::DISTANCE):
        ui->histogram->setTitle("Distance");
        ui->histogram->xAxis->setLabel(QString::fromStdString("miles"));
        break;
    case(HistogramWidget::DURATION):
        ui->histogram->setTitle("Duration");
        ui->histogram->xAxis->setLabel(QString::fromStdString("minutes"));
        break;
    case(FIELD1):
        field = Global::getInstance()->getExtraField(0);
        ui->histogram->setTitle(field.screenName);
        ui->histogram->xAxis->setLabel(field.axisLabel);
        break;
    case(FIELD2):
        field = Global::getInstance()->getExtraField(1);
        ui->histogram->setTitle(field.screenName);
        ui->histogram->xAxis->setLabel(field.axisLabel);
        break;
    case(FIELD3):
        field = Global::getInstance()->getExtraField(2);
        ui->histogram->setTitle(field.screenName);
        ui->histogram->xAxis->setLabel(field.axisLabel);
        break;
    case(FIELD4):
        field = Global::getInstance()->getExtraField(3);
        ui->histogram->setTitle(field.screenName);
        ui->histogram->xAxis->setLabel(field.axisLabel);
        break;
    default:
        cout << "ERROR: Invalid histogram attribute" << endl;
    }

    // create empty bar chart objects:
    groupPlots.clear();
    map<Group, map<PlotAttribute,vector<HistBin> > >::iterator it;
    QCPBars* prevPlot = NULL;

    for(it = groupHistograms.begin() ; it != groupHistograms.end() ; ++it){
        Group group = it->first;
        QCPBars *barPlot;
        if(groupPlots.count(group) > 0){
            barPlot = groupPlots[group];
            barPlot->clearData();
        }
        else{
            barPlot = new QCPBars(ui->histogram->xAxis, ui->histogram->yAxis);
            groupPlots[group] = barPlot;
            ui->histogram->addPlottable(barPlot);
            //
            QPen pen;
            pen.setWidthF(1.2);
            //barPlot->setName("Fossil fuels");
            pen.setColor(group.getColor());
            barPlot->setPen(pen);
            barPlot->setBrush(Qt::NoBrush);
            //            if(prevPlot != NULL)
            //                barPlot->moveAbove(prevPlot);
        }
        prevPlot = barPlot;
    }

    // prepare x axis with country labels:
    QVector<double> ticks;
    QVector<QString> labels;
    pair<float,float> dataBounds = histogramDataBounds[_plotAttribute];
    float binSize = (dataBounds.second - dataBounds.first)/numberOfBins;
    for(int i = 0 ; i < numberOfBins ; ++i){
        float minBin = dataBounds.first + i*binSize;
        float maxBin = minBin + binSize;
        float binMiddle = minBin - binSize/2;

        //qDebug() << "Size Bin " << i << " = " << binMiddle;

        ticks << 0.75*(i+1);

        labels << QString("[%1,%2]").arg(minBin, 0, 'f', 1).arg(maxBin, 0, 'f', 1);

        // stringstream ss;
        // ss << "[" << minBin << "," << maxBin << "]";

        // labels << QString::fromStdString(ss.str());//number((minBin+maxBin)/2);
    }

    ui->histogram->xAxis->setAutoTicks(false);
    ui->histogram->xAxis->setAutoTickLabels(false);
    ui->histogram->xAxis->setTickVector(ticks);
    ui->histogram->xAxis->setTickVectorLabels(labels);
    ui->histogram->xAxis->setTickLabelRotation(0);
    ui->histogram->xAxis->setSubTickCount(0);
    ui->histogram->xAxis->setTickLength(0, 4);
    ui->histogram->xAxis->setGrid(false);
    ui->histogram->xAxis->setRange(0, numberOfBins + 1);

    // prepare y axis:
    ui->histogram->yAxis->setPadding(5); // a bit more space to the left border
    ui->histogram->yAxis->setLabel("Frequency");
    ui->histogram->yAxis->setSubGrid(true);
    QPen gridPen;
    gridPen.setStyle(Qt::SolidLine);
    gridPen.setColor(QColor(0, 0, 0, 25));
    ui->histogram->yAxis->setGridPen(gridPen);
    gridPen.setStyle(Qt::DotLine);
    ui->histogram->yAxis->setSubGridPen(gridPen);

    // Add data:
    float maxCount = -1;
    for(it = groupHistograms.begin() ; it != groupHistograms.end() ; ++it){
        //
        QVector<double> data;
        Group group = it->first;
        map<PlotAttribute, std::vector<HistBin> >& groupHistograms = it->second;
        vector<HistBin>& currentPlot = groupHistograms[_plotAttribute];
        for(int i = 0 ; i < currentPlot.size() ; ++i){
            data << currentPlot.at(i).freq;
            if(currentPlot.at(i).freq > maxCount)
                maxCount = currentPlot.at(i).freq;
        }

        QCPBars* barPlot = groupPlots[group];
        barPlot->setData(ticks,data);
    }

    ui->histogram->yAxis->setRange(0, maxCount + 1);

    //    // setup legend:
    //    customPlot->legend->setVisible(true);
    //    customPlot->legend->setPositionStyle(QCPLegend::psTop);
    //    customPlot->legend->setBrush(QColor(255, 255, 255, 200));
    //    QPen legendPen;
    //    legendPen.setColor(QColor(130, 130, 130, 200));
    //    customPlot->legend->setBorderPen(legendPen);
    //    QFont legendFont = font();
    //    legendFont.setPointSize(10);
    //    customPlot->legend->setFont(legendFont);

    ui->histogram->setRangeDrag(Qt::Horizontal|Qt::Vertical);
    ui->histogram->setRangeZoom(Qt::Horizontal|Qt::Vertical);
    ui->histogram->setInteractions(QCustomPlot::iRangeDrag | QCustomPlot::iRangeZoom | QCustomPlot::iSelectAxes
                                   | QCustomPlot::iSelectTitle);

    ui->histogram->replot();

    _yMin = 0;
    _yMax = maxCount+1;
}

void HistogramWidget::recomputePlots(){
    //
    computeHistograms();

    //
    updatePlots();

    //
    updateControlValues();

    // update all plots if it is synchronize
    if (Coordinator::instance()->containsHist(this)){
        Coordinator::instance()->notifyAll();
    }
}

void HistogramWidget::updateYRange(float min, float max)
{
  ui->histogram->yAxis->setRange(min, max);
  ui->histogram->replot();
}

void HistogramWidget::changeHistAttribute(QString selectedText){
    if(!selectedText.compare(QString::fromAscii("Fare Amount"))){
        _plotAttribute = HistogramWidget::FARE_AMOUNT;
    }
    else if(!selectedText.compare(QString::fromAscii("Tip"))){
        _plotAttribute = HistogramWidget::TIP_AMOUNT;
    }
    else if(!selectedText.compare(QString::fromAscii("Distance"))){
        _plotAttribute = HistogramWidget::DISTANCE;
    }
    else if(!selectedText.compare(QString::fromAscii("Duration"))){
        _plotAttribute = HistogramWidget::DURATION;
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
    if (Coordinator::instance()->containsHist(this)) {
        Coordinator::instance()->setHistsAttribute(_plotAttribute);
        Coordinator::instance()->notifyAll();
    }

    updatePlots();
}

void HistogramWidget::setPlotAttribute(HistogramWidget::PlotAttribute attrib)
{
  _plotAttribute = attrib;
  this->ui->attributeComboBox->setCurrentIndex(attrib);
}

void HistogramWidget::updateNumBins(){
    setNumberOfBins(ui->numBinsSpinBox->value());

    recomputePlots();
}

int HistogramWidget::getNumberOfBins()
{
  return this->numberOfBins;
}

void HistogramWidget::setNumberOfBins(int v){
    numberOfBins = v;
}

void HistogramWidget::mousePress(QMouseEvent* e){
    // if an axis is selected, only allow the direction of that axis to be dragged
    // if no axis is selected, both directions may be dragged



    if (ui->histogram->xAxis->selected().testFlag(QCPAxis::spAxis))
        ui->histogram->setRangeDrag(ui->histogram->xAxis->orientation());
    else if (ui->histogram->yAxis->selected().testFlag(QCPAxis::spAxis))
        ui->histogram->setRangeDrag(ui->histogram->yAxis->orientation());
    else
        ui->histogram->setRangeDrag(Qt::Horizontal|Qt::Vertical);
}

void HistogramWidget::mouseDouble(QMouseEvent *e){
    double x = ui->histogram->xAxis->pixelToCoord(e->x());
    double y = ui->histogram->yAxis->pixelToCoord(e->y());

    //cout << "mouse " << x << " " << y << endl;

    std::map<Group,QCPBars*>::iterator it;
    for(it = groupPlots.begin() ; it != groupPlots.end() ; ++it){
        QCPBars* b = it->second;
        float barWidth = b->width();

        //qDebug() << "QCPBAR " << b->width();
        const QCPBarDataMap* datamap = b->data();

        QCPBarDataMap::const_iterator it = datamap->begin();
        QCPBarDataMap::const_iterator itEnd = datamap->end();

        for(; it != itEnd ; ++it){
            double key = it.key();
            double minBin = key - barWidth/2;
            double maxBin = key + barWidth/2;
            QCPBarData data = it.value();
            double valuef = data.value;
            //cout << "   testing " << minBin << " " << maxBin << " " << key << " " << valuef << endl;
            if(y <= valuef && minBin <= x && x <= maxBin){
                ui->histogram->addSelection(minBin,maxBin);
                break;
            }
        }

    }
}

void HistogramWidget::mouseWheel(){
    // if an axis is selected, only allow the direction of that axis to be zoomed
    // if no axis is selected, both directions may be zoomed

    if (ui->histogram->xAxis->selected().testFlag(QCPAxis::spAxis))
        ui->histogram->setRangeZoom(ui->histogram->xAxis->orientation());
    else if (ui->histogram->yAxis->selected().testFlag(QCPAxis::spAxis))
        ui->histogram->setRangeZoom(ui->histogram->yAxis->orientation());
    else
        ui->histogram->setRangeZoom(Qt::Horizontal|Qt::Vertical);
}

bool satisfySelections(float value, const QList<IntervalSelection> &selections) {
  for (int i=0; i<selections.count(); i++)
    if (selections.at(i).first<=value && value<=selections.at(i).second)
      return true;
  return selections.count()==0;
}

void HistogramWidget::joinSelectedTrips(KdTrip::TripSet *out)
{
  QList<IntervalSelection> selections = this->ui->histogram->getSelections();

  if(this->selectionGraph == NULL)
    return;

  KdTrip::TripSet filtered;

  bool buildGlobalPlot = (this->selectionGraph->isEmpty());
  set<Group> groups;
  map<Group,vector<SelectionGraphNode*> > mapGroupToNodes;
  map<Group,vector<SelectionGraphEdge*> > mapGroupToEdges;
  this->selectionGraph->groupNodesAndEdgeByColor(groups,mapGroupToNodes,mapGroupToEdges);
  set<Group>::iterator groupIterator;

  //
  KdTrip::TripSet::iterator it;

  for(it = out->begin() ; it != out->end() ; ++it) {
    const KdTrip::Trip *trip = (*it);

    // only use valid trips
    if(trip->dropoff_time < trip->pickup_time)
      continue;

    if(buildGlobalPlot) {
      float tripAttribValue = getTripValue(trip,this->_plotAttribute);
      if (satisfySelections(tripAttribValue, selections))
        filtered.insert(trip);
    }
    else{
      for(groupIterator = groups.begin() ; groupIterator != groups.end() ; ++groupIterator){
        Group currentGroup = *groupIterator;
        if(this->tripSatisfiesConstraints(*it, mapGroupToNodes[currentGroup],mapGroupToEdges[currentGroup])){
          float tripAttribValue = getTripValue(trip,this->_plotAttribute);

          if (satisfySelections(tripAttribValue, selections))
            filtered.insert(trip);
        }
      }
    }
  }
  out->swap(filtered);
}

QString HistogramWidget::getAttributeDescription()
{
  QString desc;
  QList<IntervalSelection> selections = this->ui->histogram->getSelections();
  if (!selections.empty()) {
    desc = this->ui->histogram->title() + QString(": ");
    for (int i=0; i<selections.count(); i++) {
      if (i>0)
        desc.append(" , ");
      desc.append(QString("[%1 - %2]")
                  .arg(selections.at(i).first)
                  .arg(selections.at(i).second));
    }
  }
  return desc;
}

