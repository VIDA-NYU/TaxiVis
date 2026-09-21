// End-to-end interaction cost measurement.
//
// The responsiveness review measured index queries, path containment, and set
// insertion, but not plot aggregation, layer preparation, or painting. This
// benchmark closes that gap: for representative selections it reports the cost
// of each stage a user waits through, so the decision to keep or drop worker
// threads rests on end-to-end numbers rather than on index timings alone.
//
// Not a regression test; it is not registered with CTest. Run it directly:
//   TAXIVIS_DATA=<index> ./interaction_bench
#include "geographicalviewwidget.h" // GLEW must precede Qt OpenGL headers.
#include "temporalseriesplotwidget.h"
#include "histogramwidget.h"
#include "scatterplotwidget.h"
#include "global.h"
#include <QApplication>
#include <QElapsedTimer>
#include <QTest>
#include <cstdio>
#include <stdexcept>

static void require(bool value, const char *message) {
    if (!value) throw std::runtime_error(message);
}

// Wait until every plot widget has published its computation, so the reported
// figure is the latency a user sees rather than the time to hand work off.
static void settle() {
    auto busy = [] {
        for (auto widget : QApplication::allWidgets()) {
            if (auto p=qobject_cast<TemporalSeriesPlotWidget*>(widget); p && p->computationBusy()) return true;
            if (auto p=qobject_cast<HistogramWidget*>(widget); p && p->computationBusy()) return true;
            if (auto p=qobject_cast<ScatterPlotWidget*>(widget); p && p->computationBusy()) return true;
        }
        return false;
    };
    require(QTest::qWaitFor([&] { return !busy(); }, 120000), "Computation timed out");
    QApplication::processEvents();
}

template<class F> static double timed(F work) {
    QElapsedTimer t; t.start();
    work();
    settle();
    return t.nsecsElapsed()/1e6;
}

static void scenario(const char *name, QDateTime start, QDateTime end, bool region, size_t cap) {
    SelectionGraph graph;
    if (region) {
        // A drawn pickup region, as produced by the rectangle tool.
        QPainterPath path; path.addRect(QRectF(40.745, -73.99, 0.02, 0.02));
        graph.addNode(new Selection(path, Selection::START));
    }
    KdTrip::TripSet trips;
    QElapsedTimer t; t.start();
    Global::getInstance()->queryData(&graph, start, end, trips);
    double query = t.nsecsElapsed()/1e6;
    size_t found = trips.size();
    while (trips.size() > cap) trips.erase(trips.begin());

    TemporalSeriesPlotWidget temporal;
    HistogramWidget histogram;
    ScatterPlotWidget scatter;
    for (auto *w : {(QWidget*)&temporal, (QWidget*)&histogram, (QWidget*)&scatter}) w->resize(1000, 420);

    double temporalMs = timed([&] {
        temporal.setSelectedTripsRepository(&trips);
        temporal.setSelectionGraph(&graph);
        temporal.setDateTimes(start, end);
        temporal.setNumberOfBins(10);
    });
    double histogramMs = timed([&] {
        histogram.setSelectedTripsRepository(&trips);
        histogram.setSelectionGraph(&graph);
        histogram.setNumberOfBins(10);
        histogram.recomputePlots();
    });
    double scatterMs = timed([&] {
        scatter.setSelectedTripsRepository(&trips);
        scatter.setSelectionGraph(&graph);
        scatter.recomputePlots();
    });
    // Painting stays on the GUI thread regardless of the threading decision.
    QElapsedTimer p; p.start();
    temporal.grab(); histogram.grab(); scatter.grab();
    double paint = p.nsecsElapsed()/1e6;

    // The QCustomPlot migration disabled adaptive scatter sampling to dodge an
    // upstream assertion on coincident points. Measure what that costs at paint
    // time, since scatter painting dominates the heavy selections.
    // grab() alone can blit QCustomPlot's cached buffer, so force a real
    // replot for both measurements and compare like with like.
    double unsampled = -1, sampled = -1;
    if (auto *plot = scatter.findChild<QCustomPlot*>(); plot && plot->graphCount()) {
        auto replot = [&](bool adaptive) {
            plot->graph(0)->setAdaptiveSampling(adaptive);
            QElapsedTimer s; s.start();
            plot->replot(QCustomPlot::rpImmediateRefresh);
            return s.nsecsElapsed()/1e6;
        };
        unsampled = replot(false);
        sampled = replot(true);
        plot->graph(0)->setAdaptiveSampling(false);
    }

    printf("%-34s %9zu %9.1f %9.1f %9.1f %9.1f %9.1f %9.1f %9.1f\n",
           name, trips.size(), query, temporalMs, histogramMs, scatterMs, paint, unsampled, sampled);
    if (found != trips.size())
        printf("%-34s (query returned %zu; capped to %zu for plotting)\n", "", found, trips.size());
}

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    try {
        Global::getInstance();
        auto day = QDateTime(QDate(2013,1,13), QTime(0,0));
        printf("%-34s %9s %9s %9s %9s %9s %9s %9s %9s\n",
               "scenario", "trips", "query", "temporal", "histogr", "scatter", "paint", "sc.replot", "+adapt");
        printf("%s\n", QString(114, '-').toUtf8().constData());
        scenario("1 hour, whole city",        day, day.addSecs(3600),  false, 1000000);
        scenario("1 hour, drawn region",      day, day.addSecs(3600),  true,  1000000);
        scenario("1 day, whole city",         day, day.addDays(1),     false, 1000000);
        scenario("1 day, drawn region",       day, day.addDays(1),     true,  1000000);
        printf("\nAll times in ms. query = index + spatial filter + set build.\n"
               "temporal/histogr/scatter = end-to-end until the plot is published.\n"
               "paint = grab() of all three widgets on the GUI thread.\n"
               "sc.replot/+adapt = forced scatter replot, without and with adaptive sampling.\n");
        return 0;
    } catch (const std::exception &error) {
        fprintf(stderr, "interaction_bench failed: %s\n", error.what());
        return 1;
    }
}
