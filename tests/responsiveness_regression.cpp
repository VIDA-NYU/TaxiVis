#include "geographicalviewwidget.h"
#include "viewwidget.h"
#include "QMapTileWidget.hpp"
#include "temporalseriesplotwidget.h"
#include "histogramwidget.h"
#include "scatterplotwidget.h"
#include "extendedplotwidget.h"
#include "extendedhistogram.h"
#include "TimeExplorationDialog.hpp"
#include "HistogramDialog.hpp"
#include "layers/HeatMap.hpp"
#include "layers/TripLocationLOD.hpp"
#include "layers/GridMap.hpp"
#include "coordinator.h"
#include "global.h"
#include <QApplication>
#include <QElapsedTimer>
#include <QSemaphore>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QTest>
#include <QThreadPool>
#include <QTimer>
#include <atomic>
#include <iostream>
#include <stdexcept>

static void require(bool ok, const char *message) { if (!ok) throw std::runtime_error(message); }
template<class F> static void await(F ready) { require(QTest::qWaitFor(ready, 30000), "Background operation timed out"); }
static bool plotsIdle(QObject &root) {
    for (auto p : root.findChildren<TemporalSeriesPlotWidget*>()) if (p->computationBusy()) return false;
    for (auto p : root.findChildren<HistogramWidget*>()) if (p->computationBusy()) return false;
    for (auto p : root.findChildren<ScatterPlotWidget*>()) if (p->computationBusy()) return false;
    return true;
}
static void runnerChecks() {
    LatestTask<int> job;
    auto gate=std::make_shared<QSemaphore>();
    auto started=std::make_shared<std::atomic_bool>(false);
    int delivered=0, deliveries=0;
    job.submit([gate,started](const Cancellation &) { *started=true; gate->acquire(); return 1; },
               [&](int n) { delivered=n; ++deliveries; });
    await([&] { return started->load(); });
    job.submit([](const Cancellation &) { throw std::runtime_error("Coalesced work must not execute"); return 2; },
               [&](int n) { delivered=n; ++deliveries; });
    job.submit([](const Cancellation &) { return 3; }, [&](int n) { delivered=n; ++deliveries; });
    gate->release();
    await([&] { return !job.isBusy(); });
    require(delivered==3 && deliveries==1, "Stale result applied or pending work not coalesced");
    QString error;
    job.onError=[&](const QString &s) { error=s; };
    job.submit([](const Cancellation &)->int { throw std::runtime_error("expected failure"); }, [](int) {});
    await([&] { return !job.isBusy(); });
    require(error=="expected failure", "Worker error not delivered");
    auto closing=new LatestTask<int>;
    *started=false;
    closing->submit([gate,started](const Cancellation &) { *started=true; gate->acquire(); return 1; },
                    [&](int) { ++deliveries; });
    await([&] { return started->load(); });
    QElapsedTimer clock; clock.start(); delete closing;
    require(clock.elapsed()<100, "Closing blocked on a worker");
    gate->release();
    QThreadPool::globalInstance()->waitForDone();
    QApplication::processEvents();
    require(deliveries==1, "Callback reached destroyed controller");
    std::cout << "Runner: coalescing, stale rejection, errors, nonblocking destruction\n";
}
static void snapshotChecks(QDateTime start) {
    SelectionGraph graph;
    auto dataset=Global::getInstance()->dataset();
    const auto baseline=QueryManager::query(dataset,SelectionSnapshot::capture(&graph),start,start.addSecs(3600),Cancellation());
    require(!baseline.empty(),"Snapshot fixture is empty");
    const auto trip=*baseline.begin();
    QPainterPath pickup,dropoff;
    pickup.addRect(trip->pickup_lat-.001,trip->pickup_long-.001,.002,.002);
    dropoff.addRect(trip->dropoff_lat-.001,trip->dropoff_long-.001,.002,.002);
    auto a=graph.addNode(new Selection(pickup,Selection::START));
    auto b=graph.addNode(new Selection(dropoff,Selection::END));
    graph.addEdge(a,b)->setGroup(Group(Qt::red));
    const auto snapshot=SelectionSnapshot::capture(&graph);
    size_t expected=0;
    for(auto record : baseline) if(pickup.contains(QPointF(record->pickup_lat,record->pickup_long)) &&
                                   dropoff.contains(QPointF(record->dropoff_lat,record->dropoff_long))) ++expected;
    LatestTask<KdTrip::TripSet> job;
    size_t actual=0;
    job.submit([dataset,snapshot,start](const Cancellation &cancel) {
        return QueryManager::query(dataset,snapshot,start,start.addSecs(3600),cancel);
    },[&](KdTrip::TripSet result) { actual=result.size(); });
    SelectionGraph empty; graph.assign(&empty); // destroy the original nodes while work is pending
    await([&] { return !job.isBusy(); });
    require(actual==expected && actual>0,"Spatial snapshot changed with live graph");
    auto copy=baseline;
    copy.clear();
    require(!baseline.empty(),"Trip snapshot shares mutable storage");

    // Snapshots flatten selection geometry to polygons so workers never share
    // QPainterPath's lazily built bounds cache. Containment must stay identical,
    // including for a non-convex outline where the fill rule actually matters.
    QPolygonF concave;
    concave << QPointF(0,0) << QPointF(4,0) << QPointF(4,4) << QPointF(3,4)
            << QPointF(3,1) << QPointF(1,1) << QPointF(1,4) << QPointF(0,4);
    QPainterPath star; star.addPolygon(concave); star.closeSubpath();
    for (QPainterPath *shape : {&pickup,&dropoff,&star}) {
        const Region region=Region::from(*shape);
        const QRectF box=shape->boundingRect().adjusted(-.001,-.001,.001,.001);
        size_t inside=0, probes=0;
        for (int i=0;i<=60;++i) for (int j=0;j<=60;++j) {
            const QPointF probe(box.left()+box.width()*i/60.0, box.top()+box.height()*j/60.0);
            // Skip points within rounding distance of an edge, where exact-boundary
            // classification is not meaningful for either representation.
            if (std::abs(shape->boundingRect().left()-probe.x())<1e-9) continue;
            ++probes;
            if (shape->contains(probe)) ++inside;
            require(region.contains(probe)==shape->contains(probe),
                    "Flattened selection changed point containment");
        }
        require(probes>0 && inside>0,"Containment probe grid missed the shape");
    }
    std::cout << "Snapshots: spatial edge constraints, destroyed source graph, copy-on-write storage,"
                 " polygon containment equivalence\n";
}
class TestHeat : public HeatMap {
public:
    using HeatMap::HeatMap;
    QImage image() const { return textureImage; }
};
class TestLocations : public TripLocationLOD {
public:
    using TripLocationLOD::TripLocationLOD;
    size_t size() const { return vertices.size(); }
};
static void layerChecks(QDateTime start) {
    auto storage=std::make_shared<std::vector<KdTrip::Trip>>(64);
    KdTrip::TripSet trips; trips.keepAlive(storage);
    for (auto &trip : *storage) {
        trip.pickup_lat=trip.dropoff_lat=40.75; trip.pickup_long=trip.dropoff_long=-73.98;
        trip.pickup_time=start.toSecsSinceEpoch(); trip.dropoff_time=trip.pickup_time+60;
        trips.insert(&trip);
    }
    const auto snapshot=trips;
    SelectionGraph graph;
    GeographicalViewWidget geo;
    geo.setSelectedTripsRepository(&trips); geo.setSelectionGraph(&graph);
    geo.setSelectionTime(start,start.addSecs(3600));
    TestHeat heat(&geo); heat.setResolution(QSize(128,128)); heat.setEnabled(true); heat.updateData();
    TestLocations locations(&geo); locations.updateData();
    NumTripsGridMap grid(&geo); grid.loadGrid(QString(DATA_DIR)+"neighborhoods.txt"); grid.setEnabled(true); grid.updateData();
    trips.clear();
    heat.updateData(); locations.updateData(); grid.updateData();
    await([&] { return !heat.computationBusy() && !locations.computationBusy() && !grid.computationBusy(); });
    require(heat.getMaxValue()==0 && locations.size()==0, "Layer accepted superseded trips");
    require(grid.getCellValueRange().y()==0, "Grid accepted superseded trips");
    trips=snapshot; heat.updateData(); locations.updateData(); grid.updateData();
    await([&] { return !heat.computationBusy() && !locations.computationBusy() && !grid.computationBusy(); });
    require(heat.getMaxValue()==64 && locations.size()==256, "Layer snapshot aggregation incorrect");
    require(!heat.image().isNull(), "Heat texture not prepared");
    require(grid.getCellValueRange().y()==64, "Grid trip count incorrect");
    heat.setNormalized(true); heat.setMaxValue(128);
    await([&] { return !heat.computationBusy(); });
    auto doomed=new TestHeat(&geo); doomed->setEnabled(true); doomed->updateData(); delete doomed;
    std::cout << "Layers: rapid replacement, empty sets, heat normalization, destruction\n";
}
static void integrationChecks(QDateTime start) {
    QTemporaryDir output;
    require(output.isValid(), "Temporary directory unavailable");
    ViewWidget first, second;
    auto geo=qobject_cast<GeographicalViewWidget*>(first.mapWidget());
    auto other=qobject_cast<GeographicalViewWidget*>(second.mapWidget());
    Coordinator::instance()->addView(&first); Coordinator::instance()->addView(&second);
    QSignalSpy commits(geo,&GeographicalViewWidget::datasetUpdated);
    int heartbeats=0; QTimer heartbeat; heartbeat.setInterval(1);
    QObject::connect(&heartbeat,&QTimer::timeout,[&] { ++heartbeats; }); heartbeat.start();
    for (int i=0;i<20;++i) first.updateTimes(start.addSecs(i*60),start.addSecs(3600+i*60));
    first.updateTimes(start,start.addSecs(3600));
    second.updateTimes(start,start.addSecs(7200));
    require(!first.exportToFile(output.filePath("premature.csv")), "Export accepted an uncommitted query");
    await([&] { return !geo->queryBusy() && !other->queryBusy() && plotsIdle(first) && plotsIdle(second); });
    require(commits.count()==1, "Rapid queries committed stale intermediate results");
    SelectionGraph empty;
    auto expected=QueryManager::query(Global::getInstance()->dataset(),SelectionSnapshot::capture(&empty),start,start.addSecs(3600),Cancellation());
    require(geo->getSelectedTrips()->size()==expected.size() && !expected.empty(), "Latest query differs from reference");
    for (auto trip : expected) require(geo->getSelectedTrips()->count(trip), "Query snapshot membership differs");
    auto left=first.timeSeriesWidget()->findChild<ExtendedPlotWidget*>();
    auto right=second.timeSeriesWidget()->findChild<ExtendedPlotWidget*>();
    require(left->yAxis->range()==right->yAxis->range(), "Linked time axes diverged after asynchronous commits");
    auto lh=first.histogramWidget()->findChild<ExtendedHistogram*>();
    auto rh=second.histogramWidget()->findChild<ExtendedHistogram*>();
    require(lh->yAxis->range()==rh->yAxis->range(), "Linked histogram axes diverged");
    require(heartbeats>0,"Event loop did not run while querying");
    // Export captures the committed set. A subsequent query cannot change rows.
    QSignalSpy exported(&first,&ViewWidget::exportFinished);
    require(first.exportToFile(output.filePath("snapshot.csv")),"Export was not started");
    first.updateTimes(start.addDays(10000),start.addDays(10000).addSecs(60));
    await([&] { return !first.exportBusy() && !geo->queryBusy() && plotsIdle(first); });
    require(exported.count()==1,"Export did not finish");
    QFile csv(output.filePath("snapshot.csv")); require(csv.open(QIODevice::ReadOnly),"CSV missing");
    const auto rows=csv.readAll().split('\n');
    require(size_t(rows.size()-2)==expected.size(),"CSV rows changed after a later query");
    require(geo->getSelectedTrips()->empty(),"Latest empty query was not committed");
    // A histogram dialog cannot restore its baseline after an external query.
    first.updateTimes(start,start.addSecs(3600));
    await([&] { return !geo->queryBusy() && plotsIdle(first); });
    HistogramDialog filter(geo,4);
    first.updateTimes(start.addDays(10000),start.addDays(10000).addSecs(60));
    await([&] { return !geo->queryBusy() && plotsIdle(first); });
    filter.reject(); require(geo->getSelectedTrips()->empty(),"Old dialog restored a stale baseline");
    // Rapid plot edits preserve the latest bin count across linked refreshes.
    for(int i=2;i<12;++i) second.timeSeriesWidget()->setNumberOfBins(i);
    await([&] { return plotsIdle(first) && plotsIdle(second); });
    require(right->graph(0)->data()->size()==11,"Latest plot bin edit lost");
    // Exploration owns both mapped slices and synthetic, time-aligned records.
    DateTimeList ranges; ranges << DateTimePair(start,start.addSecs(3600)) << DateTimePair(start.addSecs(3600),start.addSecs(7200));
    QMetaObject::invokeMethod(&first,"exploreInTime",Qt::DirectConnection,Q_ARG(DateTimeList,ranges));
    require(first.explorationBusy(),"Exploration did not start asynchronously");
    // A second request coalesces the first and replaces its progress dialog.
    QMetaObject::invokeMethod(&first,"exploreInTime",Qt::DirectConnection,Q_ARG(DateTimeList,ranges));
    first.cancelExploration();
    QThreadPool::globalInstance()->waitForDone(); QApplication::processEvents();
    require(first.findChildren<TimeExplorationDialog*>().empty(),"Canceled exploration published a dialog");
    QMetaObject::invokeMethod(&first,"exploreInTime",Qt::DirectConnection,Q_ARG(DateTimeList,ranges));
    await([&] { return !first.explorationBusy(); });
    auto dialogs=first.findChildren<TimeExplorationDialog*>();
    require(dialogs.size()==1,"Exploration result missing");
    auto dialog=dialogs.front(); dialog->hide();
    await([&] { return plotsIdle(*dialog); });
    require(dialog->findChildren<GeographicalViewWidget*>().size()==2,"Exploration slices missing");
    delete dialog;
    // Close a view with query, plotting, and export work pending.
    auto closing=new ViewWidget;
    closing->updateTimes(start,start.addDays(31));
    closing->close();
    delete closing;
    auto exporting=new ViewWidget;
    exporting->updateTimes(start,start.addSecs(3600));
    auto exportGeo=qobject_cast<GeographicalViewWidget*>(exporting->mapWidget());
    await([&] { return !exportGeo->queryBusy(); });
    const auto canceledPath=output.filePath("closed.csv");
    require(exporting->exportToFile(canceledPath),"Close-during-export setup failed");
    exporting->close();
    delete exporting;
    QThreadPool::globalInstance()->waitForDone(); QApplication::processEvents();
    require(!QFile::exists(canceledPath),"Closed view published a canceled export");
    Coordinator::instance()->removeView(&first); Coordinator::instance()->removeView(&second);
    await([&] { return plotsIdle(first) && plotsIdle(second); });
    std::cout << "Integration: rapid edits, linked views, committed export, stale dialogs, exploration, close during work; heartbeats=" << heartbeats << '\n';
}
// Offscreen Qt cannot create a GL viewport. Exercise real controllers/layers,
// but suppress viewport paints; native GL drawing is checked separately.
class NoGLPaint : public QObject {
    bool eventFilter(QObject *object,QEvent *event) override {
        return event->type()==QEvent::Paint && qobject_cast<QOpenGLWidget*>(object);
    }
};
int main(int argc,char **argv) {
    QApplication app(argc,argv);
    NoGLPaint noGL; app.installEventFilter(&noGL);
    try {
        runnerChecks();
        Global::getInstance();
        const QDateTime start(QDate(2013,1,13),QTime(0,0));
        snapshotChecks(start);
        layerChecks(start);
        integrationChecks(start);
        QThreadPool::globalInstance()->waitForDone();
        std::cout << "Responsiveness checks passed\n";
        return 0;
    } catch (const std::exception &error) { std::cerr << error.what() << '\n'; return 1; }
}
