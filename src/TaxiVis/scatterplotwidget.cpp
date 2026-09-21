#include "scatterplotwidget.h"
#include "ui_scatterplotwidget.h"
#include <QDebug>
#include <cassert>
#include <iostream>
#include "global.h"

using namespace std;

ScatterPlotWidget::ScatterPlotWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ScatterPlotWidget),
    selectedTrips(NULL),
    selectionGraph(NULL)
{
    ui->setupUi(this);

    attrib1 = getAttrib(ui->comboBox->currentText());
    attrib2 = getAttrib(ui->comboBox_2->currentText());

    //
    Global* global = Global::getInstance();
    int numExtraFields = global->numExtraFields();
    for(int i = 0 ; i < numExtraFields ; ++i){
        ExtraField field = global->getExtraField(i);
        if(field.active){
            ui->comboBox->addItem(field.screenName);
            ui->comboBox_2->addItem(field.screenName);
        }
    }

    // connect slots that takes care that when an axis is selected, only that direction can be dragged and zoomed:
    connect(ui->customPlot, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress()));
    connect(ui->customPlot, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel()));

    //
    updatePlot();
}

ScatterPlotWidget::~ScatterPlotWidget()
{
    delete ui;
}

void ScatterPlotWidget::mousePress(){
    // if an axis is selected, only allow the direction of that axis to be dragged
    // if no axis is selected, both directions may be dragged

    if (ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
        ui->customPlot->axisRect()->setRangeDrag(ui->customPlot->xAxis->orientation());
    else if (ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
        ui->customPlot->axisRect()->setRangeDrag(ui->customPlot->yAxis->orientation());
    else
        ui->customPlot->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
}

void ScatterPlotWidget::mouseWheel(){
    // if an axis is selected, only allow the direction of that axis to be zoomed
    // if no axis is selected, both directions may be zoomed

    if (ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
        ui->customPlot->axisRect()->setRangeZoom(ui->customPlot->xAxis->orientation());
    else if (ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
        ui->customPlot->axisRect()->setRangeZoom(ui->customPlot->yAxis->orientation());
    else
        ui->customPlot->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
}

void ScatterPlotWidget::setAttribute1(QString s){
    if(QString::compare(s,"Fare Amount") == 0)
        attrib1 = ScatterPlotWidget::FARE_AMOUNT;
    else if(QString::compare(s,"Tip Amount") == 0)
        attrib1 = ScatterPlotWidget::TIP_AMOUNT;
    else if(QString::compare(s,"Distance") == 0)
        attrib1 = ScatterPlotWidget::DISTANCE;
    else if(QString::compare(s,"Duration") == 0)
        attrib1 = ScatterPlotWidget::DURATION;
    else if(QString::compare(s,"Tolls Amount") == 0)
        attrib1 = ScatterPlotWidget::TOLL_AMOUNT;
    else if(QString::compare(s,"Average Speed") == 0)
        attrib1 = ScatterPlotWidget::AVG_SPEED;
    else if(QString::compare(s,"Hour of Day") == 0)
        attrib1 = ScatterPlotWidget::TIME_OF_DAY;
    else{
        //find extra field
        int fieldIndex = Global::getInstance()->getIndexByScreenName(s);
        if(fieldIndex == -1){
            qDebug() << "Invalide Attribute";
            assert(false);
        }
        else{
            switch(fieldIndex){
            case 0:
                attrib1 = FIELD1;
                break;
            case 1:
                attrib1 = FIELD2;
                break;
            case 2:
                attrib1 = FIELD3;
                break;
            case 3:
                attrib1 = FIELD4;
                break;
            default:
                assert(false);
                break;
            }
        }
    }

    recomputePlots();
}

void ScatterPlotWidget::setAttribute2(QString s){
    if(QString::compare(s,"Fare Amount") == 0)
        attrib2 = ScatterPlotWidget::FARE_AMOUNT;
    else if(QString::compare(s,"Tip Amount") == 0)
        attrib2 = ScatterPlotWidget::TIP_AMOUNT;
    else if(QString::compare(s,"Distance") == 0)
        attrib2 = ScatterPlotWidget::DISTANCE;
    else if(QString::compare(s,"Duration") == 0)
        attrib2 = ScatterPlotWidget::DURATION;
    else if(QString::compare(s,"Tolls Amount") == 0)
        attrib2 = ScatterPlotWidget::TOLL_AMOUNT;
    else if(QString::compare(s,"Average Speed") == 0)
        attrib2 = ScatterPlotWidget::AVG_SPEED;
    else if(QString::compare(s,"Hour of Day") == 0)
        attrib2 = ScatterPlotWidget::TIME_OF_DAY;
    else{
        //find extra field
        int fieldIndex = Global::getInstance()->getIndexByScreenName(s);
        if(fieldIndex == -1){
            qDebug() << "Invalide Attribute";
            assert(false);
        }
        else{
            switch(fieldIndex){
            case 0:
                attrib2 = FIELD1;
                break;
            case 1:
                attrib2 = FIELD2;
                break;
            case 2:
                attrib2 = FIELD3;
                break;
            case 3:
                attrib2 = FIELD4;
                break;
            default:
                break;
            }
        }
    }

    recomputePlots();
}

bool ScatterPlotWidget::tripSatisfiesEdge(const KdTrip::Trip *trip, SelectionGraphEdge* edge){
    return (edge->getTail()->getSelection()->contains(QPointF(trip->pickup_lat,trip->pickup_long)) &&
            edge->getHead()->getSelection()->contains(QPointF(trip->dropoff_lat,trip->dropoff_long)));
}

bool ScatterPlotWidget::tripSatisfiesConstraints(const KdTrip::Trip *trip,
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

void ScatterPlotWidget::updatePlot(){
    ui->customPlot->clearGraphs();//clearPlottables();

    if(suspended || selectedTrips == NULL || selectionGraph == NULL)
        return;

    //
    ExtraField field;
    switch(attrib1){
    case(FARE_AMOUNT):
        ui->customPlot->xAxis->setLabel(QString::fromStdString("Fare Amount (US$)"));
        break;
    case(TIP_AMOUNT):
        ui->customPlot->xAxis->setLabel(QString::fromStdString("Tip Amount (US$)"));
        break;
    case(DURATION):
        ui->customPlot->xAxis->setLabel(QString::fromStdString("Duration (min)"));
        break;
    case(DISTANCE):
        ui->customPlot->xAxis->setLabel(QString::fromStdString("Distance (mi)"));
        break;
    case(TOLL_AMOUNT):
        ui->customPlot->xAxis->setLabel(QString::fromStdString("Toll Amount (US$)"));
        break;
    case(AVG_SPEED):
        ui->customPlot->xAxis->setLabel(QString::fromStdString("Avg Speed (mi/h)"));
        break;
    case(TIME_OF_DAY):
        ui->customPlot->xAxis->setLabel(QString::fromStdString("Hour of day"));
        break;
    case(FIELD1):
        field = Global::getInstance()->getExtraField(0);
        ui->customPlot->xAxis->setLabel(field.axisLabel);
        break;
    case(FIELD2):
        field = Global::getInstance()->getExtraField(1);
        ui->customPlot->xAxis->setLabel(field.axisLabel);
        break;
    case(FIELD3):
        field = Global::getInstance()->getExtraField(2);
        ui->customPlot->xAxis->setLabel(field.axisLabel);
        break;
    case(FIELD4):
        field = Global::getInstance()->getExtraField(3);
        ui->customPlot->xAxis->setLabel(field.axisLabel);
        break;
    default:
        assert(false);
        break;
    }

    //
    switch(attrib2){
    case(FARE_AMOUNT):
        ui->customPlot->yAxis->setLabel(QString::fromStdString("Fare Amount (US$)"));
        break;
    case(TIP_AMOUNT):
        ui->customPlot->yAxis->setLabel(QString::fromStdString("Tip Amount (US$)"));
        break;
    case(DURATION):
        ui->customPlot->yAxis->setLabel(QString::fromStdString("Duration (min)"));
        break;
    case(DISTANCE):
        ui->customPlot->yAxis->setLabel(QString::fromStdString("Distance (mi)"));
        break;
    case(TOLL_AMOUNT):
        ui->customPlot->yAxis->setLabel(QString::fromStdString("Toll Amount (US$)"));
        break;
    case(AVG_SPEED):
        ui->customPlot->yAxis->setLabel(QString::fromStdString("Avg Speed (mi/h)"));
        break;
    case(TIME_OF_DAY):
        ui->customPlot->yAxis->setLabel(QString::fromStdString("Hour of day"));
        break;
    case(FIELD1):
        field = Global::getInstance()->getExtraField(0);
        ui->customPlot->yAxis->setLabel(field.axisLabel);
        break;
    case(FIELD2):
        field = Global::getInstance()->getExtraField(1);
        ui->customPlot->yAxis->setLabel(field.axisLabel);
        break;
    case(FIELD3):
        field = Global::getInstance()->getExtraField(2);
        ui->customPlot->yAxis->setLabel(field.axisLabel);
        break;
    case(FIELD4):
        field = Global::getInstance()->getExtraField(3);
        ui->customPlot->yAxis->setLabel(field.axisLabel);
        break;
    default:
        assert(false);
        break;
    }

    const auto trips = *selectedTrips;
    const auto selection = SelectionSnapshot::capture(selectionGraph);
    const auto xAttribute = attrib1, yAttribute = attrib2;
    computeJob.submit([trips, selection, xAttribute, yAttribute](const Cancellation &cancel) {
        std::map<Group, QVector<QCPGraphData>> points;
        for (const auto &group : selection.groups) points[group.first];
        double xmin=INFINITY,xmax=-INFINITY,ymin=INFINITY,ymax=-INFINITY;
        size_t n=0;
        for (auto trip : trips) {
            if ((n++ & 1023)==0) cancel.check();
            const auto p = getCoords(trip, xAttribute, yAttribute);
            for (auto &group : points) if (selection.matches(group.first, trip)) {
                group.second.append(QCPGraphData(p.x(),p.y()));
                xmin=std::min(xmin,p.x()); xmax=std::max(xmax,p.x());
                ymin=std::min(ymin,p.y()); ymax=std::max(ymax,p.y());
            }
        }
        ScatterData data;
        for (auto &group : points) {
            cancel.check();
            auto container = QSharedPointer<QCPGraphDataContainer>::create();
            size_t comparisons=0;
            std::sort(group.second.begin(),group.second.end(),[&](const QCPGraphData &a,const QCPGraphData &b) {
                if ((comparisons++ & 4095)==0) cancel.check();
                return a.key<b.key;
            });
            container->set(group.second, true);
            data.groups[group.first] = container;
        }
        auto range=[](double low,double high) {
            if (!std::isfinite(low) || !std::isfinite(high)) return QCPRange(0,1);
            if (low==high) return QCPRange(low-.5,high+.5);
            return QCPRange(low,high);
        };
        data.xRange=range(xmin,xmax); data.yRange=range(ymin,ymax);
        return data;
    }, [this, global=selection.global](ScatterData data) {
        ui->customPlot->clearGraphs();
        for (const auto &entry : data.groups) {
            auto graph = ui->customPlot->addGraph();
            auto color = entry.first.getColor();
            color.setAlphaF(global ? 0.05 : 0.3);
            graph->setPen(QPen(color));
            // Keep coincident points: alpha blending represents density.
            graph->setAdaptiveSampling(false);
            graph->setLineStyle(QCPGraph::lsNone);
            graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 10));
            graph->setData(entry.second);
        }
        ui->customPlot->xAxis->setRange(data.xRange);
        ui->customPlot->yAxis->setRange(data.yRange);
        ui->customPlot->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
        ui->customPlot->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
        ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
                                        QCP::iSelectLegend | QCP::iSelectPlottables | QCP::iSelectOther);
        ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
    });
}

void ScatterPlotWidget::updateAttributes(){

}

ScatterPlotWidget::ScatterPlotAttributes ScatterPlotWidget::getAttrib(QString s){
    if(QString::compare(s,"Fare Amount") == 0)
        return FARE_AMOUNT;
    else if(QString::compare(s,"Tip Amount") == 0)
        return TIP_AMOUNT;
    else if(QString::compare(s,"Distance") == 0)
        return DISTANCE;
    else if(QString::compare(s,"Duration") == 0)
        return DURATION;
    else if(QString::compare(s,"Hour of Day") == 0)
        return TIME_OF_DAY;        
    else{
        //find extra field
        int fieldIndex = Global::getInstance()->getIndexByScreenName(s);
        if(fieldIndex == -1){
            qDebug() << "Invalide Attribute";
            assert(false);
        }
        else{
            switch(fieldIndex){
            case 0:
                return FIELD1;
                break;
            case 1:
                return FIELD2;
                break;
            case 2:
                return FIELD3;
                break;
            case 3:
                return FIELD4;
                break;
            default:
                assert(false);
            }
        }
    }
}

QPointF ScatterPlotWidget::getCoords(const KdTrip::Trip *trip, ScatterPlotAttributes attrib1, ScatterPlotAttributes attrib2){
    //
    qreal coord1;
    //
    time_t t = trip->pickup_time;
    struct tm localTime;
    struct tm* st_tm = localtime_r(&t, &localTime);
    assert(st_tm != NULL);
    //cout << "   st_tm " << st_tm->tm_hour << " " << (st_tm->tm_yday + 1900) << endl;

    switch(attrib1){
    case(FARE_AMOUNT):
        coord1 = trip->fare_amount/100.0;
        break;
    case(TIP_AMOUNT):
        coord1 = trip->tip_amount/100.0;
        break;
    case(DURATION):
        coord1 = (trip->dropoff_time - trip->pickup_time)/60.0;
        break;
    case(DISTANCE):
        coord1 = trip->distance;//100.0;
        break;
    case(TOLL_AMOUNT):
        coord1 = trip->tolls_amount/100.0;
        break;
    case(AVG_SPEED):
        if(trip->dropoff_time == trip->pickup_time)
            coord1 = 0;
        else
            coord1 = (trip->distance * 36)/(trip->dropoff_time - trip->pickup_time);
        break;
    case(TIME_OF_DAY):
        coord1 = st_tm->tm_hour;
        break;
    case(FIELD1):
        coord1 = trip->field1;
        break;
    case(FIELD2):
        coord1 = trip->field2;
        break;
    case(FIELD3):
        coord1 = trip->field3;
        break;
    case(FIELD4):
        coord1 = trip->field4;
        break;
    default:
        assert(false);
        break;
    }

    //
    qreal coord2;

    switch(attrib2){
    case(FARE_AMOUNT):
        coord2 = trip->fare_amount/100.0;
        break;
    case(TIP_AMOUNT):
        coord2 = trip->tip_amount/100.0;
        break;
    case(DURATION):
        coord2 = (trip->dropoff_time - trip->pickup_time)/60.0;
        break;
    case(DISTANCE):
        coord2 = trip->distance;///100.0;
        break;
    case(TOLL_AMOUNT):
        coord2 = trip->tolls_amount/100.0;
        break;
    case(AVG_SPEED):
        if(trip->dropoff_time == trip->pickup_time)
            coord2 = 0;
        else
            coord2 = (trip->distance * 36)/(trip->dropoff_time - trip->pickup_time);
        break;
    case(TIME_OF_DAY):
        coord2 = st_tm->tm_hour;
        break;
    case(FIELD1):
        coord2 = trip->field1;
        break;
    case(FIELD2):
        coord2 = trip->field2;
        break;
    case(FIELD3):
        coord2 = trip->field3;
        break;
    case(FIELD4):
        coord2 = trip->field4;
        break;
    default:
        assert(false);
        break;
    }

    //
    return QPointF(coord1,coord2);
}

void ScatterPlotWidget::setSelectedTripsRepository(KdTrip::TripSet *v){
    selectedTrips = v;
}

void ScatterPlotWidget::setSelectionGraph(SelectionGraph* g){
    selectionGraph = g;
}

void ScatterPlotWidget::recomputePlots(){
    updatePlot();
}
