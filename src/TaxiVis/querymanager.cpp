#include "querymanager.h"
#include <cassert>
#include <iostream>
#include <QDebug>
#include <QFile>
#include <cstdlib>

using namespace std;

QueryManager::QueryManager(){
    //create KDTrip
    // Dataset resolution order:
    //   1. TAXIVIS_DATA environment variable (full path to a .kdtrip file)
    //   2. DATA_DIR/2012_merged.kdtrip (full dataset, if present)
    //   3. DATA_DIR/sample_merged_1.kdtrip (bundled sample)
    std::string fname;
    const char *envData = getenv("TAXIVIS_DATA");
    if (envData != NULL && envData[0] != '\0') {
        fname = envData;
    } else {
        fname = string(DATA_DIR)+"2012_merged.kdtrip";
        if (!QFile::exists(QString::fromStdString(fname))) {
            fname = string(DATA_DIR)+"sample_merged_1.kdtrip";
        }
    }
    qDebug() << "Loading taxi trip data from:" << QString::fromStdString(fname);
    if (!QFile::exists(QString::fromStdString(fname))) {
        qCritical() << "Dataset not found:" << QString::fromStdString(fname);
        qCritical() << "Set TAXIVIS_DATA to the path of a .kdtrip file, or place one in" << DATA_DIR;
        exit(1);
    }
    kdtrip = std::make_shared<KdTrip>(fname);

    // Structural validation already collected these statistics in one pass.
    size_t tripCount = kdtrip->tripCount();
    uint32_t minTime = kdtrip->minPickupTime(), maxTime = kdtrip->maxDropoffTime();

    qDebug() << "Taxi trip data loaded successfully";
    qDebug() << "  Number of trips:" << tripCount;

    if (tripCount > 0) {
        QDateTime minDate = QDateTime::fromSecsSinceEpoch(minTime);
        QDateTime maxDate = QDateTime::fromSecsSinceEpoch(maxTime);
        qDebug() << "  Time range:" << minDate.toString("yyyy-MM-dd HH:mm")
                 << "to" << maxDate.toString("yyyy-MM-dd HH:mm");
    }
}

QueryManager::~QueryManager() = default;

KdTrip::TripSet QueryManager::query(std::shared_ptr<const KdTrip> data,
                                    const SelectionSnapshot &selection,
                                    QDateTime start, QDateTime end, const Cancellation &cancel) {
    KdTrip::TripSet out;
    out.keepAlive(data);
    if (!start.isValid() || !end.isValid() || start > end) return out;
    auto execute = [&](const SpatialConstraint &c) {
        cancel.check();
        KdTrip::Query query;
        query.setPickupTimeInterval(start.toSecsSinceEpoch(), end.toSecsSinceEpoch());
        query.setDropoffTimeInterval(start.toSecsSinceEpoch(), end.toSecsSinceEpoch());
        if (c.hasPickup) {
            QRectF r = c.pickup.boundingRect();
            query.setPickupArea(r.left(), r.top(), r.right(), r.bottom());
        }
        if (c.hasDropoff) {
            QRectF r = c.dropoff.boundingRect();
            query.setDropoffArea(r.left(), r.top(), r.right(), r.bottom());
        }
        auto result = data->execute(query, cancel.flag.get());
        cancel.check();
        size_t visited = 0;
        for (auto it = result.begin(); it != result.end(); ++it) {
            if ((++visited % 1024) == 0) cancel.check();
            if (c.matches(it.trip())) out.insert(it.trip());
        }
    };
    if (selection.global) execute(SpatialConstraint());
    else for (const auto &group : selection.groups)
        for (const auto &constraint : group.second) execute(constraint);
    return out;
}

void QueryManager::queryData(SelectionGraph *graph, QDateTime start, QDateTime end,
                            KdTrip::TripSet &out) {
    out = query(kdtrip, SelectionSnapshot::capture(graph), start, end, Cancellation());
}
