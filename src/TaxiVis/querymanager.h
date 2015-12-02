#ifndef QUERYMANGET_H
#define QUERYMANGET_H

#include "KdTrip.hpp"
#include "SelectionGraph.h"
#include <QDateTime>

class QueryManager
{
private:
    KdTrip*        kdtrip;
public:
    QueryManager();
    ~QueryManager();
    void queryData(SelectionGraph* queryGraph, QDateTime startTime, QDateTime endTime, KdTrip::TripSet &resultSet);
};

#endif // QUERYMANGET_H
