#include "querymanager.h"
#include <cassert>
#include <iostream>
#include <QDebug>

using namespace std;

QueryManager::QueryManager(){
    //create KDTrip
    std::string fname = string(DATA_DIR)+"sample_merged_1.kdtrip";
    fname = "/home/nivan/src/TaxiVis/src/preprocess/build/myTest.kdtrip";
    kdtrip = new KdTrip(fname);
}

QueryManager::~QueryManager(){
    if(kdtrip != NULL)
        delete kdtrip;
}

void QueryManager::queryData(SelectionGraph *queryGraph, QDateTime startDateTime,
                            QDateTime endDateTime, KdTrip::TripSet &resultSet) {
    //initial setup
    assert(queryGraph != NULL);
    resultSet.clear();

    //
    QDate startDate = startDateTime.date();
    QTime startTime = startDateTime.time();
    QDate endDate   = endDateTime.date();
    QTime endTime   = endDateTime.time();

    {//if graph is empry
        if(queryGraph->isEmpty()){
            //
            KdTrip::Query query;

            //
            query.setPickupTimeInterval(query.createTime(startDate.year(),startDate.month(),startDate.day(),startTime.hour(),startTime.minute(),startTime.second()),
                                        query.createTime(endDate.year(),endDate.month(),endDate.day(),endTime.hour(),endTime.minute(),endTime.second()));
            query.setDropoffTimeInterval(query.createTime(startDate.year(),startDate.month(),startDate.day(),startTime.hour(),startTime.minute(),startTime.second()),
                                         query.createTime(endDate.year(),endDate.month(),endDate.day(),endTime.hour(),endTime.minute(),endTime.second()));

            KdTrip::QueryResult result = kdtrip->execute(query);
            KdTrip::QueryResult::iterator it;
            for (it=result.begin(); it<result.end(); ++it) {
                resultSet.insert(it.trip());
//                qDebug() << "Trip: ";
//                qDebug() << "    fare: " << it.trip()->fare_amount;
//                qDebug() << "    distance: " << it.trip()->distance;
//                qDebug() << "    field1: " << it.trip()->field1;
//                qDebug() << "    id: " << it.trip()->id_taxi;
//                qDebug() << "    tip: " << it.trip()->tip_amount;
//                qDebug() << "    payment type: " << it.trip()->payment_type;
//                qDebug() << "    passengers: " << (uint)(it.trip()->passengers);
            }

            return;
        }


    }


    //
    set<Group> groups;
    map<Group, vector<SelectionGraphNode*> > mapGroupToNodes;
    map<Group, vector<SelectionGraphEdge*> > mapGroupToEdges;
    queryGraph->groupNodesAndEdgeByColor(groups,mapGroupToNodes, mapGroupToEdges);

    //edge queries
    SelectionGraph::EdgeIterator it;
    SelectionGraph::EdgeIterator edgesBegin;
    SelectionGraph::EdgeIterator edgesEnd;
    queryGraph->getEdgeIterator(edgesBegin,edgesEnd);
    set<int> alreadyProcessedNodes;
    //int countEdgeQuery = 1;
    for(it = edgesBegin ; it != edgesEnd ; ++it){
        //cout << "Executing edge query " << countEdgeQuery++ << endl;
        SelectionGraphEdge* edge = it->second;
        SelectionGraphNode* tail = edge->getTail();
        SelectionGraphNode* head = edge->getHead();

        //
        KdTrip::Query query;
        QRectF originRect = tail->getSelection()->boundingBox();
        query.setPickupArea(originRect.x(),originRect.y(),originRect.x() + originRect.width(),originRect.y() + originRect.height());
        QRectF destinationRect = head->getSelection()->boundingBox();
        query.setDropoffArea(destinationRect.x(),destinationRect.y(),destinationRect.x() + destinationRect.width(),destinationRect.y() + destinationRect.height());

//        qDebug() << "   Origin Rect " << originRect;
//        qDebug() << "   Destination Rect " << destinationRect;

        //
        query.setPickupTimeInterval(query.createTime(startDate.year(),startDate.month(),startDate.day(),startTime.hour(),startTime.minute(),startTime.second()),
                                    query.createTime(endDate.year(),endDate.month(),endDate.day(),endTime.hour(),endTime.minute(),endTime.second()));
        query.setDropoffTimeInterval(query.createTime(startDate.year(),startDate.month(),startDate.day(),startTime.hour(),startTime.minute(),startTime.second()),
                                     query.createTime(endDate.year(),endDate.month(),endDate.day(),endTime.hour(),endTime.minute(),endTime.second()));

        KdTrip::QueryResult result = kdtrip->execute(query);
        //cout << "   Query Result " << result.size() << endl;
        //resultSet.insert(result.begin(),result.end());
        KdTrip::QueryResult::iterator it;
        for (it=result.begin(); it<result.end(); ++it) {
            //
            if(tail->getSelection()->contains(QPointF(it->pickup_lat,it->pickup_long)) &&
                    head->getSelection()->contains(QPointF(it->dropoff_lat,it->dropoff_long)))
                resultSet.insert(it.trip());
        }

        //
        alreadyProcessedNodes.insert(tail->getId());
        alreadyProcessedNodes.insert(head->getId());
    }

    //cout << "   After edge queries " << resultSet.size() << endl;

    // node queries
    SelectionGraph::NodeIterator nodesit;
    SelectionGraph::NodeIterator nodesBegin;
    SelectionGraph::NodeIterator nodesEnd;
    queryGraph->getNodeIterator(nodesBegin,nodesEnd);
    //int countNodeQuery = 1;
    for(nodesit = nodesBegin ; nodesit != nodesEnd ; ++nodesit){
        //cout << "Executing Node Query " << countNodeQuery++ << endl;
        SelectionGraphNode* node = nodesit->second;
        if(alreadyProcessedNodes.count(node->getId()) > 0)
            continue;

        //
        KdTrip::Query query;
        KdTrip::Query extraQuery;

        //time constraints
        query.setPickupTimeInterval(query.createTime(startDate.year(),startDate.month(),startDate.day(),startTime.hour(),startTime.minute(),startTime.second()),
                                    query.createTime(endDate.year(),endDate.month(),endDate.day(),endTime.hour(),endTime.minute(),endTime.second()));
        query.setDropoffTimeInterval(query.createTime(startDate.year(),startDate.month(),startDate.day(),startTime.hour(),startTime.minute(),startTime.second()),
                                     query.createTime(endDate.year(),endDate.month(),endDate.day(),endTime.hour(),endTime.minute(),endTime.second()));

        extraQuery.setPickupTimeInterval(query.createTime(startDate.year(),startDate.month(),startDate.day(),startTime.hour(),startTime.minute(),startTime.second()),
                                         query.createTime(endDate.year(),endDate.month(),endDate.day(),endTime.hour(),endTime.minute(),endTime.second()));
        extraQuery.setDropoffTimeInterval(query.createTime(startDate.year(),startDate.month(),startDate.day(),startTime.hour(),startTime.minute(),startTime.second()),
                                          query.createTime(endDate.year(),endDate.month(),endDate.day(),endTime.hour(),endTime.minute(),endTime.second()));

        //
        if(node->getSelection()->getType() == Selection::START){
            QRectF originRect = node->getSelection()->boundingBox();
            query.setPickupArea(originRect.x(),originRect.y(),originRect.x() + originRect.width(),originRect.y() + originRect.height());
        }
        else if(node->getSelection()->getType() == Selection::END){
            QRectF destinationRect = node->getSelection()->boundingBox();
            query.setDropoffArea(destinationRect.x(),destinationRect.y(),destinationRect.x() + destinationRect.width(),destinationRect.y() + destinationRect.height());
        }
        else if(node->getSelection()->getType() == Selection::START_AND_END){
            //
            QRectF originRect = node->getSelection()->boundingBox();
            query.setPickupArea(originRect.x(),originRect.y(),originRect.x() + originRect.width(),originRect.y() + originRect.height());

            //
            QRectF destinationRect = node->getSelection()->boundingBox();
            extraQuery.setDropoffArea(destinationRect.x(),destinationRect.y(),destinationRect.x() + destinationRect.width(),destinationRect.y() + destinationRect.height());
        }


        KdTrip::QueryResult result = kdtrip->execute(query);
        KdTrip::QueryResult::iterator it;
        qDebug() << "Result " << result.size() ;
        for (it=result.begin(); it<result.end(); ++it) {
            const KdTrip::Trip *trip = it.trip();

            bool startContains = node->getSelection()->contains(QPointF(trip->pickup_lat, trip->pickup_long));
            bool endContains   = node->getSelection()->contains(QPointF(trip->dropoff_lat, trip->dropoff_long));

            if(node->getSelection()->getType() == Selection::START && !startContains)
                continue;
            else if(node->getSelection()->getType() == Selection::END && !endContains)
                continue;
            else if(node->getSelection()->getType() == Selection::START_AND_END && !startContains && !endContains)
                continue;

            resultSet.insert(trip);
        }

        if(node->getSelection()->getType() == Selection::START_AND_END){
            KdTrip::QueryResult result = kdtrip->execute(extraQuery);
            KdTrip::QueryResult::iterator it;
            for (it=result.begin(); it<result.end(); ++it) {
                const KdTrip::Trip *trip = it.trip();

                bool startContains = node->getSelection()->contains(QPointF(trip->pickup_lat, trip->pickup_long));
                bool endContains   = node->getSelection()->contains(QPointF(trip->dropoff_lat, trip->dropoff_long));

                if(node->getSelection()->getType() == Selection::START && !startContains)
                    continue;
                else if(node->getSelection()->getType() == Selection::END && !endContains)
                    continue;
                else if(node->getSelection()->getType() == Selection::START_AND_END && !startContains && !endContains)
                    continue;

                resultSet.insert(trip);
            }
        }
    }
    //cout << "After node queries " << resultSet.size() << endl;
}
