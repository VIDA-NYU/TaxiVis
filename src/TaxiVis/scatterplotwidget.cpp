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

    if (ui->customPlot->xAxis->selected().testFlag(QCPAxis::spAxis))
        ui->customPlot->setRangeDrag(ui->customPlot->xAxis->orientation());
    else if (ui->customPlot->yAxis->selected().testFlag(QCPAxis::spAxis))
        ui->customPlot->setRangeDrag(ui->customPlot->yAxis->orientation());
    else
        ui->customPlot->setRangeDrag(Qt::Horizontal|Qt::Vertical);
}

void ScatterPlotWidget::mouseWheel(){
    // if an axis is selected, only allow the direction of that axis to be zoomed
    // if no axis is selected, both directions may be zoomed

    if (ui->customPlot->xAxis->selected().testFlag(QCPAxis::spAxis))
        ui->customPlot->setRangeZoom(ui->customPlot->xAxis->orientation());
    else if (ui->customPlot->yAxis->selected().testFlag(QCPAxis::spAxis))
        ui->customPlot->setRangeZoom(ui->customPlot->yAxis->orientation());
    else
        ui->customPlot->setRangeZoom(Qt::Horizontal|Qt::Vertical);
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

    if(selectedTrips == NULL)
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

    //
    bool buildGlobalPlot = (selectionGraph->isEmpty());

    set<Group> groups;
    map<Group,vector<SelectionGraphNode*> > mapGroupToNodes;
    map<Group,vector<SelectionGraphEdge*> > mapGroupToEdges;
    set<Group>::iterator groupIterator;
    selectionGraph->groupNodesAndEdgeByColor(groups,mapGroupToNodes,mapGroupToEdges);
    map<Group,QCPGraph*> mapGroupGraph;

    //
    set<Group> notEmptyGroups;
    map<Group,vector<SelectionGraphNode*> > tempMapGroupToNodes;

    if(buildGlobalPlot){
        QPen pen;
        QCPGraph* graph = ui->customPlot->addGraph();
        QColor color(0,0,0);
        mapGroupGraph[Group(color)] = graph;
        color.setAlphaF(0.05);
        pen.setColor(color);
        graph->setPen(pen);
        graph->setLineStyle(QCPGraph::lsNone);
        graph->setScatterStyle(QCP::ssDisc);
        graph->setScatterSize(10);
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

        groups.clear();
        groups = notEmptyGroups;
        mapGroupToNodes.clear();
        mapGroupToNodes = tempMapGroupToNodes;

        //
        for(groupIterator = groups.begin() ; groupIterator != groups.end() ; ++groupIterator){
            QPen pen;
            QCPGraph* graph = ui->customPlot->addGraph();
            graph->setLineStyle(QCPGraph::lsNone);
            graph->setScatterStyle(QCP::ssDisc);
            graph->setScatterSize(10);

            Group group = *groupIterator;

            QColor color = group.getColor();
            mapGroupGraph[group] = graph;
            color.setAlphaF(0.3);
            pen.setColor(color);
            graph->setPen(pen);
        }
    }

    //int numberOfTrip = selectedTrips->size();
    map<Group,pair<QVector<double>,QVector<double> > > mapGroupData;
    if(buildGlobalPlot){
        int numberOfTrips = selectedTrips->size();
        //QVector<double> x(numberOfTrips), y(numberOfTrips);
        QVector<double> x, y;
        mapGroupData[QColor(0,0,0)] = make_pair(x,y);
    }
    else{
        QVector<double> x, y;
        for(groupIterator = groups.begin() ; groupIterator != groups.end() ; ++groupIterator){
            Group group = *groupIterator;

            mapGroupData[group.getColor()] = make_pair(x,y);

        }
    }

    // add graphs with different scatter styles:
    KdTrip::TripSet::iterator it = selectedTrips->begin();
    for (; it != selectedTrips->end(); ++it) {
        const KdTrip::Trip * trip = *it;
        QPointF coords = getCoords(trip);

        if(buildGlobalPlot){
            pair<QVector<double>,QVector<double> > &data =
                    mapGroupData[QColor(0,0,0)];
            QVector<double> &x = data.first;
            QVector<double> &y = data.second;
            x << coords.x();
            y << coords.y();
        }
        else{
            for(groupIterator = groups.begin() ; groupIterator != groups.end() ; ++groupIterator){
                Group currentGroup = *groupIterator;

                assert(mapGroupToNodes.count(currentGroup) > 0 && mapGroupToEdges.count(currentGroup) > 0);

                if(tripSatisfiesConstraints(trip, mapGroupToNodes[currentGroup],mapGroupToEdges[currentGroup])){
                    pair<QVector<double>,QVector<double> > &data =
                            mapGroupData[currentGroup.getColor()];
                    QVector<double> &x = data.first;
                    QVector<double> &y = data.second;
                    x << coords.x();
                    y << coords.y();
                    continue; // make sure the point is only added once
                }
            }
        }
    }

    if(buildGlobalPlot){
        pair<QVector<double>,QVector<double> > &data =
                mapGroupData[QColor(0,0,0)];
        QVector<double> &x = data.first;
        QVector<double> &y = data.second;
        QCPGraph* graph = mapGroupGraph[QColor(0,0,0)];
        graph->setData(x, y);
        graph->rescaleAxes(true);
    }
    else{
        for(groupIterator = groups.begin() ; groupIterator != groups.end() ; ++groupIterator){
            Group currentGroup = *groupIterator;
            pair<QVector<double>,QVector<double> > &data =
                    mapGroupData[currentGroup.getColor()];
            QVector<double> &x = data.first;
            QVector<double> &y = data.second;
            QCPGraph* graph = mapGroupGraph[currentGroup.getColor()];
            graph->setData(x, y);
            graph->rescaleAxes(true);
        }
    }

    //
    int numGraphs = ui->customPlot->graphCount();
    for(int i = 0 ; i < numGraphs ; ++i){
        ui->customPlot->graph(i)->rescaleAxes(false,true);
    }
    //ui->customPlot->yAxis->scaleRange(1.1, ui->customPlot->yAxis->range().center());
    ui->customPlot->setRangeDrag(Qt::Horizontal|Qt::Vertical);
    ui->customPlot->setRangeZoom(Qt::Horizontal|Qt::Vertical);
    ui->customPlot->setInteractions(QCustomPlot::iRangeDrag | QCustomPlot::iRangeZoom | QCustomPlot::iSelectAxes |
                                    QCustomPlot::iSelectLegend | QCustomPlot::iSelectPlottables | QCustomPlot::iSelectTitle);

    //
    ui->customPlot->replot();
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

QPointF ScatterPlotWidget::getCoords(const KdTrip::Trip *trip){
    //
    qreal coord1;
    //
    time_t t = trip->pickup_time;
    struct tm* st_tm =  localtime (&t);
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
            coord1 = 0;
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
