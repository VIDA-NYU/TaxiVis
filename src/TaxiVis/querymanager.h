#ifndef QUERYMANGET_H
#define QUERYMANGET_H

#include "KdTrip.hpp"
#include "SelectionGraph.h"
#include <QDateTime>
#include "SelectionSnapshot.hpp"
#include "AsyncTask.hpp"

class QueryManager
{
private:
    std::shared_ptr<KdTrip> kdtrip;
public:
    QueryManager();
    ~QueryManager();
    std::shared_ptr<const KdTrip> dataset() const { return kdtrip; }
    static KdTrip::TripSet query(std::shared_ptr<const KdTrip> data, const SelectionSnapshot &selection,
                                QDateTime start, QDateTime end, const Cancellation &cancel);
    void queryData(SelectionGraph* queryGraph, QDateTime startTime, QDateTime endTime, KdTrip::TripSet &resultSet);
};

#endif // QUERYMANGET_H
