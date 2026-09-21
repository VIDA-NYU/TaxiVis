#include "geographicalviewwidget.h" // GLEW must precede Qt OpenGL headers.
#include "temporalseriesplotwidget.h"
#include "histogramwidget.h"
#include "scatterplotwidget.h"
#include "extendedplotwidget.h"
#include "extendedhistogram.h"
#include "global.h"
#include "HistogramDialog.hpp"
#include "TemporalSeriesDialog.hpp"
#include <QApplication>
#include <QComboBox>
#include <QDir>
#include <QSignalSpy>
#include <QSpinBox>
#include <QTest>
#include <cmath>
#include <iostream>
#include <stdexcept>

static void require(bool value, const char *message) {
    if (!value) throw std::runtime_error(message);
}
static void settle() {
    auto busy=[] {
        for (auto widget : QApplication::allWidgets()) {
            if (auto p=qobject_cast<TemporalSeriesPlotWidget*>(widget); p && p->computationBusy()) return true;
            if (auto p=qobject_cast<HistogramWidget*>(widget); p && p->computationBusy()) return true;
            if (auto p=qobject_cast<ScatterPlotWidget*>(widget); p && p->computationBusy()) return true;
        }
        return false;
    };
    require(QTest::qWaitFor([&] { return !busy(); }, 30000), "Plot computation timed out");
    QApplication::processEvents();
}
static void equal(double actual, double expected, const char *message) {
    if (std::abs(actual-expected) > 0.001) {
        std::cerr << message << ": " << actual << " vs " << expected << '\n';
        throw std::runtime_error(message);
    }
}
static double sum(QCPGraph *graph) {
    double value = 0;
    for (const auto &point : *graph->data()) value += point.value;
    return value;
}
static double sum(QCPBars *bars) {
    double value = 0;
    for (const auto &point : *bars->data()) value += point.value;
    return value;
}
static void render(QWidget &widget, const QString &name) {
    widget.resize(1000, 420);
    widget.show();
    QApplication::processEvents();
    const QString dir = qEnvironmentVariable("TAXIVIS_PLOT_OUTPUT");
    if (!dir.isEmpty()) {
        QDir().mkpath(dir);
        require(widget.grab().save(dir + '/' + name + ".png"), "Unable to save plot rendering");
    }
}

static void checkPlots(KdTrip::TripSet &trips, SelectionGraph &graph,
                       QDateTime start, QDateTime end, bool synthetic) {
    TemporalSeriesPlotWidget temporal;
    HistogramWidget histogram;
    ScatterPlotWidget scatter;
    temporal.setSelectedTripsRepository(&trips);
    temporal.setSelectionGraph(&graph);
    temporal.setDateTimes(start, end);
    temporal.setNumberOfBins(4);
    histogram.setSelectedTripsRepository(&trips);
    histogram.setSelectionGraph(&graph);
    histogram.setNumberOfBins(4);
    histogram.recomputePlots();
    scatter.setSelectedTripsRepository(&trips);
    scatter.setSelectionGraph(&graph);
    scatter.recomputePlots();
    settle();
    auto timePlot = temporal.findChild<ExtendedPlotWidget*>();
    auto histPlot = histogram.findChild<ExtendedHistogram*>();
    auto scatterPlot = scatter.findChild<QCustomPlot*>();
    require(timePlot && histPlot && scatterPlot, "Missing plot widget");
    require(timePlot->graphCount() == 1, "Expected one temporal graph");
    equal(sum(timePlot->graph(0)), trips.size(), "Temporal trip count");
    require(timePlot->graph(0)->data()->size() == 4, "Temporal bin count");
    auto ticker = qSharedPointerDynamicCast<QCPAxisTickerDateTime>(timePlot->xAxis->ticker());
    require(!ticker.isNull() && ticker->dateTimeSpec() == Qt::LocalTime, "Local date/time ticker");
    require(ticker->dateTimeFormat() == "hh:mm\nMMM dd", "Date label format");
    auto bars = qobject_cast<QCPBars*>(histPlot->plottable(0));
    require(bars && bars->data()->size() == 4, "Histogram data container/bin count");
    size_t validCount = 0;
    for (const auto *trip : trips) if (trip->dropoff_time >= trip->pickup_time) ++validCount;
    equal(sum(bars), validCount, "Histogram frequency total");
    auto textTicker = qSharedPointerDynamicCast<QCPAxisTickerText>(histPlot->xAxis->ticker());
    require(!textTicker.isNull() && textTicker->ticks().size() == 4, "Histogram text labels");
    require(histPlot->title() == "Fare Amount", "Histogram title");
    require(scatterPlot->graphCount() == 1, "Scatter graph count");
    require(scatterPlot->graph(0)->data()->size() == int(trips.size()), "Scatter point count including duplicate keys");
    require(scatterPlot->graph(0)->scatterStyle().shape() == QCPScatterStyle::ssDisc, "Scatter style");
    equal(scatterPlot->graph(0)->scatterStyle().size(), 10, "Scatter size");
    require(!scatterPlot->graph(0)->adaptiveSampling(), "Preserve scatter density and coincident points");
    render(temporal, synthetic ? "synthetic-time" : "january-time");
    render(histogram, synthetic ? "synthetic-histogram" : "january-histogram");
    render(scatter, synthetic ? "synthetic-scatter" : "january-scatter");

    if (synthetic) {
        for (const auto &point : *timePlot->graph(0)->data()) equal(point.value, 1, "Known temporal bin");
        for (const auto &point : *bars->data()) equal(point.value, 1, "Known histogram bin");
        require(textTicker->ticks().first() == "[5.0,8.8]", "First histogram label");
        // Double-click a known bin through the production hit-testing slot.
        QPointF hit(histPlot->xAxis->coordToPixel(0.75), histPlot->yAxis->coordToPixel(0.5));
        QMouseEvent doubleClick(QEvent::MouseButtonDblClick, hit, hit, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
        require(QMetaObject::invokeMethod(&histogram, "mouseDouble", Qt::DirectConnection,
                                         Q_ARG(QMouseEvent*, &doubleClick)), "Histogram double-click handler");
        KdTrip::TripSet filtered = trips;
        histogram.joinSelectedTrips(&filtered);
        require(filtered.size() == 1 && (*filtered.begin())->fare_amount == 500, "Histogram selection conversion/filtering");
        histPlot->clearSelections();
        // Verify right-drag time selection still emits an interval after the
        // QCP layout gained a title row and axis rectangles.
        QSignalSpy selection(timePlot, &ExtendedPlotWidget::timeIntervalChanged);
        QPoint a(timePlot->xAxis->coordToPixel(start.toSecsSinceEpoch()+600), timePlot->axisRect()->center().y());
        QPoint b(timePlot->xAxis->coordToPixel(start.toSecsSinceEpoch()+1800), a.y());
        QTest::mousePress(timePlot, Qt::RightButton, Qt::NoModifier, a);
        QTest::mouseMove(timePlot, b);
        QTest::mouseRelease(timePlot, Qt::RightButton, Qt::NoModifier, b);
        require(selection.count() == 1, "Time selection signal");
        auto args = selection.takeFirst();
        require(std::abs(args[0].toDateTime().secsTo(start.addSecs(600))) < 10, "Selection start time");
        require(std::abs(args[1].toDateTime().secsTo(start.addSecs(1800))) < 10, "Selection end time");
        // Axis selection must restrict dragging to the selected direction.
        timePlot->xAxis->setSelectedParts(QCPAxis::spAxis);
        QMetaObject::invokeMethod(&temporal, "mousePress", Qt::DirectConnection);
        require(timePlot->axisRect()->rangeDrag() == Qt::Horizontal, "Axis drag restriction");
        timePlot->yAxis->setSelectedParts(QCPAxis::spAxis);
        timePlot->xAxis->setSelectedParts(QCPAxis::spNone);
        QMetaObject::invokeMethod(&temporal, "mouseWheel", Qt::DirectConnection);
        require(timePlot->axisRect()->rangeZoom() == Qt::Vertical, "Axis zoom restriction");
        temporal.setPlotAttribute(TemporalSeriesPlotWidget::FARE_AMOUNT);
        temporal.updatePlots();
        equal(sum(timePlot->graph(0)), 50, "Fare aggregation after attribute change");
        temporal.setNumberOfBins(2);
        settle();
        require(timePlot->graph(0)->data()->size() == 2, "Temporal bins after edit");
        auto bins = histogram.findChild<QSpinBox*>("numBinsSpinBox");
        require(bins != nullptr, "Histogram bin editor");
        bins->setValue(2);
        QMetaObject::invokeMethod(bins, "editingFinished", Qt::DirectConnection);
        settle();
        require(qobject_cast<QCPBars*>(histPlot->plottable(0))->data()->size() == 2, "Histogram bins after edit");
        auto combo = scatter.findChild<QComboBox*>("comboBox_2");
        require(combo != nullptr, "Scatter attribute combo");
        combo->setCurrentText("Distance");
        require(scatterPlot->yAxis->label().contains("Distance"), "Scatter attribute signal");
        scatter.findChild<QComboBox*>("comboBox")->setCurrentText("Tip Amount");
        settle();
        require(scatterPlot->graph(0)->data()->size() == 4, "Duplicate scatter x keys retained");
        for (const auto &point : *scatterPlot->graph(0)->data()) equal(point.key, 1, "Scatter attribute values");
    }
    // A second group must get its own graph and preserve totals/colours.
    QPainterPath region;
    region.addRect(QRectF(39, -75, 3, 3));
    auto node = graph.addNode(new Selection(region, Selection::START));
    node->setGroup(Group(Qt::red));
    auto second = graph.addNode(new Selection(region, Selection::START));
    second->setGroup(Group(Qt::blue));
    temporal.setPlotAttribute(TemporalSeriesPlotWidget::NUMBER_OF_TRIPS);
    temporal.recomputePlots();
    histogram.recomputePlots();
    scatter.recomputePlots();
    settle();
    require(timePlot->graphCount() == 2 && histPlot->plottableCount() == 2 && scatterPlot->graphCount() == 2,
            "Separate plots for separate groups");
    if (synthetic) {
        for (int i=0; i<2; ++i) {
            equal(sum(timePlot->graph(i)), trips.size(), "Group temporal count");
            equal(sum(qobject_cast<QCPBars*>(histPlot->plottable(i))), trips.size(), "Group histogram count");
        }
    }
    require(timePlot->graph(0)->pen().color() != timePlot->graph(1)->pen().color(), "Distinct group colours");
    // Repeated replots must not keep inserting title rows or bars.
    for (int i=0; i<3; ++i) { histogram.recomputePlots(); temporal.recomputePlots(); }
    settle();
    require(histPlot->plotLayout()->rowCount() == 2 && timePlot->plotLayout()->rowCount() == 2, "Stable title layout");
    require(histPlot->plottableCount() == 2, "Stable plottable ownership");
    std::cout << (synthetic ? "Synthetic" : "January") << " plots checked: " << trips.size() << " trips\n";
}

static void checkDialogs(KdTrip::TripSet &trips, QDateTime start, QDateTime end) {
    SelectionGraph graph;
    GeographicalViewWidget geo;
    geo.setSelectedTripsRepository(&trips);
    geo.setSelectionGraph(&graph);
    geo.setSelectionTime(start, end);
    TemporalSeriesDialog temporal(&geo, 4);
    settle();
    auto timePlots = temporal.findChildren<ExtendedPlotWidget*>();
    require(timePlots.size() >= 2, "Temporal exploration plots");
    QCPRange timeRange(start.addSecs(600).toSecsSinceEpoch(), start.addSecs(1800).toSecsSinceEpoch());
    timePlots.front()->xAxis->setRange(timeRange);
    for (auto *plot : timePlots) {
        equal(plot->xAxis->range().lower, timeRange.lower, "Linked temporal lower range");
        equal(plot->xAxis->range().upper, timeRange.upper, "Linked temporal upper range");
    }
    HistogramDialog histogram(&geo, 4);
    settle();
    auto histPlots = histogram.findChildren<ExtendedHistogram*>();
    require(histPlots.size() == 4, "Histogram exploration plots");
    histPlots.front()->xAxis->setRange(0.5, 2.5);
    for (auto *plot : histPlots) {
        equal(plot->xAxis->range().lower, 0.5, "Linked histogram lower range");
        equal(plot->xAxis->range().upper, 2.5, "Linked histogram upper range");
    }
    std::cout << "Exploration dialogs: linked axes verified\n";
}

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    try {
        require(QCUSTOMPLOT_VERSION == 0x020101, "Unexpected QCustomPlot version");
        Global::getInstance();
        auto start = QDateTime(QDate(2013,1,13), QTime(0,0));
        std::vector<KdTrip::Trip> records(4);
        KdTrip::TripSet synthetic;
        for (size_t i=0; i<records.size(); ++i) {
            auto &trip = records[i];
            trip.pickup_time = start.toSecsSinceEpoch() + 300 + i*900;
            trip.dropoff_time = trip.pickup_time + 60;
            trip.pickup_lat = trip.dropoff_lat = 40.7f;
            trip.pickup_long = trip.dropoff_long = -74.f;
            trip.fare_amount = 500*(i+1);
            trip.tip_amount = 100;
            trip.distance = 100*(i+1);
            synthetic.insert(&trip);
        }
        SelectionGraph syntheticGraph;
        checkPlots(synthetic, syntheticGraph, start, start.addSecs(3600), true);
        checkDialogs(synthetic, start, start.addSecs(3600));
        SelectionGraph januaryGraph;
        KdTrip::TripSet january;
        Global::getInstance()->queryData(&januaryGraph, start, start.addDays(1).addSecs(-1), january);
        require(!january.empty(), "January query is empty");
        // Bound scatter rendering cost; selection comes from the actual loaded
        // dataset, including the full-month file when TAXIVIS_DATA is supplied.
        while (january.size() > 10000) january.erase(january.begin());
        checkPlots(january, januaryGraph, start, start.addDays(1), false);
        std::cout << "QCustomPlot " << QCUSTOMPLOT_VERSION_STR << "; Qt " << qVersion() << '\n';
        return 0;
    } catch (const std::exception &error) {
        std::cerr << "Plot regression failed: " << error.what() << '\n';
        return 1;
    }
}
