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
    connect(ui->attributeComboBox,SIGNAL(currentTextChanged(QString)),this,SLOT(changeHistAttribute(QString)));

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
        return;
    }

    //get reference values
    pair<float,float> attribBounds = histogramDataBounds[_plotAttribute];


    QCPBars* b = groupPlots.begin()->second;
    float barWidth = b->width();

    const auto data = b->data();
    if (data->isEmpty()) return;
    double minBin = data->constBegin()->key - barWidth/2;
    double maxBin = (data->constEnd()-1)->key + barWidth/2;

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

void HistogramWidget::updatePlots(){
    if (groupHistograms.empty()) return;
    const int numberOfBins=int(groupHistograms.begin()->second.at(_plotAttribute).size());

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
            barPlot->data()->clear();
        }
        else{
            barPlot = new QCPBars(ui->histogram->xAxis, ui->histogram->yAxis);
            groupPlots[group] = barPlot;
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

    QSharedPointer<QCPAxisTickerText> ticker(new QCPAxisTickerText);
    ticker->addTicks(ticks, labels);
    ui->histogram->xAxis->setTicker(ticker);
    ui->histogram->xAxis->setTickLabelRotation(0);
    ui->histogram->xAxis->setSubTicks(false);
    ui->histogram->xAxis->setTickLength(0, 4);
    ui->histogram->xAxis->grid()->setVisible(false);
    ui->histogram->xAxis->setRange(0, numberOfBins + 1);

    // prepare y axis:
    ui->histogram->yAxis->setPadding(5); // a bit more space to the left border
    ui->histogram->yAxis->setLabel("Frequency");
    ui->histogram->yAxis->grid()->setSubGridVisible(true);
    QPen gridPen;
    gridPen.setStyle(Qt::SolidLine);
    gridPen.setColor(QColor(0, 0, 0, 25));
    ui->histogram->yAxis->grid()->setPen(gridPen);
    gridPen.setStyle(Qt::DotLine);
    ui->histogram->yAxis->grid()->setSubGridPen(gridPen);

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

    ui->histogram->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
    ui->histogram->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
    ui->histogram->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes
                                   | QCP::iSelectOther);

    ui->histogram->replot(QCustomPlot::rpQueuedReplot);

    _yMin = 0;
    _yMax = maxCount+1;
}

void HistogramWidget::recomputePlots(){
    if (suspended || !selectedTrips || !selectionGraph) return;
    const auto trips = *selectedTrips;
    const auto selection = SelectionSnapshot::capture(selectionGraph);
    const int bins = std::max(1, numberOfBins);
    computeJob.submit([trips, selection, bins](const Cancellation &cancel) {
        HistogramData data;
        for (int i=FARE_AMOUNT; i<=FIELD4; ++i)
            data.bounds[PlotAttribute(i)] = {std::numeric_limits<float>::max(), std::numeric_limits<float>::lowest()};
        size_t n=0;
        for (auto trip : trips) {
            if ((n++ & 1023)==0) cancel.check();
            if (trip->dropoff_time < trip->pickup_time) continue;
            for (auto &entry : data.bounds) updateBounds(entry.second, getTripValue(trip, entry.first));
        }
        std::map<PlotAttribute, std::vector<HistBin>> prototype;
        for (auto &entry : data.bounds) {
            auto &bounds = entry.second;
            if (bounds.first > bounds.second) bounds = {0,1};
            else if (bounds.first == bounds.second) bounds = {bounds.first-1,bounds.second+1};
            const float width = (bounds.second-bounds.first)/bins;
            auto &hist = prototype[entry.first];
            hist.resize(bins);
            for (int i=0; i<bins; ++i) { hist[i].minBin = bounds.first+i*width; hist[i].maxBin = bounds.first+(i+1)*width; }
        }
        for (const auto &group : selection.groups) data.groups[group.first] = prototype;
        n=0;
        for (auto trip : trips) {
            if ((n++ & 1023)==0) cancel.check();
            if (trip->dropoff_time < trip->pickup_time) continue;
            for (auto &group : data.groups) if (selection.matches(group.first, trip)) {
                for (auto &hist : group.second) {
                    const auto bounds = data.bounds.at(hist.first);
                    int index = int((getTripValue(trip,hist.first)-bounds.first)/(bounds.second-bounds.first)*bins);
                    index = std::clamp(index, 0, bins-1);
                    hist.second[index].freq++;
                }
            }
        }
        return data;
    }, [this](HistogramData data) {
        groupHistograms = std::move(data.groups);
        histogramDataBounds = std::move(data.bounds);
        updatePlots();
        updateControlValues();
        if (Coordinator::instance()->containsHist(this)) Coordinator::instance()->notifyAll();
    });
}

void HistogramWidget::updateYRange(float min, float max)
{
  ui->histogram->yAxis->setRange(min, max);
  ui->histogram->replot(QCustomPlot::rpQueuedReplot);
}

void HistogramWidget::changeHistAttribute(QString selectedText){
    if(!selectedText.compare(QString::fromLatin1("Fare Amount"))){
        _plotAttribute = HistogramWidget::FARE_AMOUNT;
    }
    else if(!selectedText.compare(QString::fromLatin1("Tip"))){
        _plotAttribute = HistogramWidget::TIP_AMOUNT;
    }
    else if(!selectedText.compare(QString::fromLatin1("Distance"))){
        _plotAttribute = HistogramWidget::DISTANCE;
    }
    else if(!selectedText.compare(QString::fromLatin1("Duration"))){
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
    const QSignalBlocker blocker(ui->numBinsSpinBox);
    ui->numBinsSpinBox->setValue(v);
    numberOfBins = ui->numBinsSpinBox->value();
}

void HistogramWidget::mousePress(QMouseEvent* e){
    // if an axis is selected, only allow the direction of that axis to be dragged
    // if no axis is selected, both directions may be dragged



    if (ui->histogram->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
        ui->histogram->axisRect()->setRangeDrag(ui->histogram->xAxis->orientation());
    else if (ui->histogram->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
        ui->histogram->axisRect()->setRangeDrag(ui->histogram->yAxis->orientation());
    else
        ui->histogram->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
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
        const auto datamap = b->data();

        QCPBarsDataContainer::const_iterator it = datamap->begin();
        QCPBarsDataContainer::const_iterator itEnd = datamap->end();

        for(; it != itEnd ; ++it){
            double key = it->key;
            double minBin = key - barWidth/2;
            double maxBin = key + barWidth/2;
            QCPBarsData data = *it;
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

    if (ui->histogram->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
        ui->histogram->axisRect()->setRangeZoom(ui->histogram->xAxis->orientation());
    else if (ui->histogram->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
        ui->histogram->axisRect()->setRangeZoom(ui->histogram->yAxis->orientation());
    else
        ui->histogram->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
}

bool satisfySelections(float value, const QList<IntervalSelection> &selections) {
  for (int i=0; i<selections.count(); i++)
    if (selections.at(i).first<=value && value<=selections.at(i).second)
      return true;
  return selections.count()==0;
}

std::function<bool(const KdTrip::Trip*)> HistogramWidget::filterSnapshot() const
{
    const auto intervals=ui->histogram->getSelections();
    const auto selection=SelectionSnapshot::capture(selectionGraph);
    const auto attribute=_plotAttribute;
    return [intervals,selection,attribute](const KdTrip::Trip *trip) {
        if (trip->dropoff_time<trip->pickup_time || !satisfySelections(getTripValue(trip,attribute),intervals)) return false;
        for (const auto &group : selection.groups) if (selection.matches(group.first,trip)) return true;
        return false;
    };
}

void HistogramWidget::joinSelectedTrips(KdTrip::TripSet *out)
{
    const auto accepts=filterSnapshot();
    KdTrip::TripSet filtered;
    filtered.inheritOwners(*out);
    for (auto trip : *out) if (accepts(trip)) filtered.insert(trip);
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

