// Native interaction check for the Qt 6 runtime checklist in PLAN.md.
//
// Runs the real MainWindow on the host platform with a live OpenGL viewport and
// drives it with in-process Qt events (no OS-level input injection, so it does
// not move the user's cursor). Each step prints PASS/FAIL and saves a window
// grab for visual inspection. Not a CTest case: it needs a display and GL.
//
//   TAXIVIS_DATA=... native_interaction_check [output-dir]
#include "mainwindow.h"
#include "viewwidget.h"
#include "geographicalviewwidget.h"
#include "QMapTileWidget.hpp"
#include "temporalseriesplotwidget.h"
#include "histogramwidget.h"
#include "scatterplotwidget.h"
#include "extendedhistogram.h"
#include "timeselectionwidget.h"
#include "TimeExplorationDialog.hpp"
#include "TemporalSeriesDialog.hpp"
#include "HistogramDialog.hpp"
#include "layers/TripAnimation.hpp"
#include "layers/TripLocationLOD.hpp"
#include "querymanager.h"
#include "SelectionSnapshot.hpp"
#include "global.h"
#include <QApplication>
#include <QComboBox>
#include <QDir>
#include <QInputDialog>
#include <QScreen>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QTabWidget>
#include <QTemporaryDir>
#include <QTest>
#include <QTimer>
#include <QToolButton>
#include <iostream>

static int failures = 0;
static QDir outDir;
static QWidget *mainWin = nullptr;

static void report(bool ok, const QString &what) {
    if (!ok) ++failures;
    std::cout << (ok ? "PASS  " : "FAIL  ") << what.toStdString() << std::endl;
}
static void shot(const QString &name, QWidget *w = nullptr) {
    QApplication::processEvents();
    QWidget *target = (w ? w : mainWin)->window();
    // QWidget::grab() cannot see the map: the scene paints it natively into the
    // GL viewport. Grab what the window system shows instead (on macOS this
    // needs the screen-recording permission); fall back to grab() if refused.
    target->raise();
    QTest::qWait(250);
    QPixmap image = target->screen()->grabWindow(target->winId());
    if (image.isNull()) image = target->grab();
    image.save(outDir.filePath(name + ".png"));
}
static bool plotsIdle(QObject *root) {
    for (auto p : root->findChildren<TemporalSeriesPlotWidget*>()) if (p->computationBusy()) return false;
    for (auto p : root->findChildren<HistogramWidget*>()) if (p->computationBusy()) return false;
    for (auto p : root->findChildren<ScatterPlotWidget*>()) if (p->computationBusy()) return false;
    return true;
}
static bool settle(GeographicalViewWidget *geo, QObject *root) {
    const bool ok = QTest::qWaitFor([&] { return !geo->queryBusy() && plotsIdle(root); }, 60000);
    QTest::qWait(150); // let the GL viewport repaint
    return ok;
}
static size_t reference(GeographicalViewWidget *geo, QDateTime start, QDateTime end) {
    return QueryManager::query(Global::getInstance()->dataset(),
                               SelectionSnapshot::capture(geo->getSelectionGraph()),
                               start, end, Cancellation()).size();
}
// Press at the first point, move through the rest, release at the last.
static void drag(QWidget *target, Qt::MouseButton button, Qt::KeyboardModifiers mods, const QList<QPoint> &points) {
    QTest::mousePress(target, button, mods, points.first());
    for (int i = 1; i < points.size(); ++i) {
        const QPoint a = points[i-1], b = points[i];
        for (int s = 1; s <= 8; ++s) QTest::mouseMove(target, a + (b - a) * s / 8, 2);
    }
    QTest::mouseRelease(target, button, mods, points.last());
}
static QRectF nodeBounds(SelectionGraph *graph, int index) {
    SelectionGraph::NodeIterator it, end;
    graph->getNodeIterator(it, end);
    std::advance(it, index);
    return it->second->getSelection()->getGeometry().boundingRect();
}
template<class T> static T *child(QObject *root, const char *name) {
    T *w = root->findChild<T*>(name);
    if (!w) { std::cerr << "missing widget " << name << std::endl; std::exit(2); }
    return w;
}

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    outDir = QDir(argc > 1 ? argv[1] : "native-interaction");
    outDir.mkpath(".");
    Global::getInstance();

    MainWindow window;
    mainWin = &window;
    window.show();
    if (!QTest::qWaitForWindowExposed(&window)) { std::cerr << "window not exposed\n"; return 2; }

    auto view = window.findChild<ViewWidget*>();
    auto geo = qobject_cast<GeographicalViewWidget*>(view->mapWidget());
    auto vp = geo->viewport();
    auto times = child<TimeSelectionWidget>(view, "timeSelectionWidget");
    auto graph = geo->getSelectionGraph();
    const QDateTime start(QDate(2013, 1, 13), QTime(8, 0));
    QDateTime end = start.addSecs(3600);
    const int W = vp->width(), H = vp->height();
    auto P = [&](double fx, double fy) { return QPoint(int(W * fx), int(H * fy)); };
    std::cout << "viewport " << W << "x" << H << ", window " << window.width() << "x" << window.height() << std::endl;

    // --- Query button
    times->setTimes(start, end);
    child<QPushButton>(times, "queryButton")->click();
    report(settle(geo, view) && geo->getSelectedTrips()->size() == reference(geo, start, end)
               && !geo->getSelectedTrips()->empty(),
           QString("Query button: %1 trips for %2").arg(geo->getSelectedTrips()->size()).arg(start.toString(Qt::ISODate)));
    shot("01-query");

    // --- Rectangle (Shift + right drag)
    drag(vp, Qt::RightButton, Qt::ShiftModifier, {P(.15, .25), P(.40, .60)});
    settle(geo, view);
    {
        bool ok = graph->numberOfNodes() == 1;
        if (ok) {
            const QRectF got = nodeBounds(graph, 0);
            const QRectF want = QRectF(geo->mapView()->mapToGeoLocation(P(.15, .25)),
                                       geo->mapView()->mapToGeoLocation(P(.40, .60))).normalized();
            ok = std::abs(got.left()-want.left()) < 1e-6 && std::abs(got.right()-want.right()) < 1e-6
              && std::abs(got.top()-want.top()) < 1e-6 && std::abs(got.bottom()-want.bottom()) < 1e-6;
        }
        const size_t n = geo->getSelectedTrips()->size();
        report(ok && n == reference(geo, start, end) && n > 0, QString("Rectangle region: %1 trips").arg(n));
    }
    shot("02-rectangle");

    // --- Freehand (plain right drag)
    drag(vp, Qt::RightButton, Qt::NoModifier,
         {P(.60, .20), P(.80, .22), P(.85, .45), P(.72, .55), P(.58, .42), P(.60, .20)});
    settle(geo, view);
    report(graph->numberOfNodes() == 2 && geo->getSelectedTrips()->size() == reference(geo, start, end),
           QString("Freehand region: nodes=%1, %2 trips").arg(graph->numberOfNodes()).arg(geo->getSelectedTrips()->size()));
    shot("03-freehand");

    // --- Move (left drag inside the rectangle)
    {
        const QRectF before = nodeBounds(graph, 0);
        const size_t tripsBefore = geo->getSelectedTrips()->size();
        drag(vp, Qt::LeftButton, Qt::NoModifier, {P(.27, .42), P(.32, .75)});
        settle(geo, view);
        const QRectF after = nodeBounds(graph, 0);
        const bool moved = before != after && std::abs(before.width()-after.width()) < 1e-6
                        && std::abs(before.height()-after.height()) < 1e-6;
        report(moved && graph->numberOfNodes() == 2 && geo->getSelectedTrips()->size() == reference(geo, start, end),
               QString("Move region: trips %1 -> %2").arg(tripsBefore).arg(geo->getSelectedTrips()->size()));
    }
    shot("04-move");

    // --- Link (link mode, right drag from the rectangle to the freehand region)
    child<QToolButton>(view, "linkModeButton")->click();
    drag(vp, Qt::RightButton, Qt::NoModifier, {P(.32, .75), P(.72, .35)});
    settle(geo, view);
    report(graph->numberOfEdges() == 1 && geo->getSelectedTrips()->size() == reference(geo, start, end),
           QString("Link regions: edges=%1, %2 trips").arg(graph->numberOfEdges()).arg(geo->getSelectedTrips()->size()));
    shot("05-link");
    child<QToolButton>(view, "pickupAndDropoffButton")->click();

    // --- Plot tabs and Num Bins, with the two-region selection in place
    // The time panel also has a QTabWidget named "tabWidget"; pick the plot one.
    QTabWidget *tabs = nullptr;
    for (auto t : view->findChildren<QTabWidget*>()) if (t->indexOf(view->histogramWidget()) >= 0) tabs = t;
    if (!tabs) { std::cerr << "plot tab widget not found" << std::endl; return 2; }
    // The plots react to editingFinished, so type the value and press Enter as a user would.
    auto typeInto = [](QSpinBox *spin, int value) {
        spin->setFocus();
        spin->selectAll();
        QTest::keyClicks(spin, QString::number(value));
        QTest::keyClick(spin, Qt::Key_Return);
    };
    {
        tabs->setCurrentIndex(1);
        settle(geo, view);
        auto spin = child<QSpinBox>(view->histogramWidget(), "numBinsSpinBox");
        typeInto(spin, 25);
        settle(geo, view);
        auto plot = view->histogramWidget()->findChild<ExtendedHistogram*>();
        int bars = 0, points = 0;
        for (int i = 0; plot && i < plot->plottableCount(); ++i)
            if (auto b = qobject_cast<QCPBars*>(plot->plottable(i))) { ++bars; points = qMax(points, b->data()->size()); }
        report(view->histogramWidget()->getNumberOfBins() == 25 && bars > 0 && points == 25,
               QString("Histogram tab, Num Bins=25: %1 bar series, %2 bins").arg(bars).arg(points));
        shot("06-histogram");
        tabs->setCurrentIndex(2);
        settle(geo, view);
        auto scatter = tabs->widget(2)->findChild<QCustomPlot*>();
        int scatterPoints = 0;
        for (int i = 0; scatter && i < scatter->graphCount(); ++i) scatterPoints += scatter->graph(i)->data()->size();
        // A small dataset can leave the linked selection empty; then no points is correct.
        const size_t selected = geo->getSelectedTrips()->size();
        report((scatterPoints > 0) == (selected > 0),
               QString("Scatter tab: %1 points for %2 trips").arg(scatterPoints).arg(selected));
        shot("07-scatter");
        tabs->setCurrentIndex(0);
        auto tsSpin = child<QSpinBox>(view->timeSeriesWidget(), "numBinsSpin");
        typeInto(tsSpin, 12);
        settle(geo, view);
        report(view->timeSeriesWidget()->getNumberOfBins() == 12, "Time series tab, Num Bins=12");
        shot("08-timeseries");
    }

    // --- Delete (Ctrl + click): remove the freehand region; its edge goes with it
    QTest::mouseClick(vp, Qt::LeftButton, Qt::ControlModifier, P(.72, .35));
    settle(geo, view);
    report(graph->numberOfNodes() == 1 && graph->numberOfEdges() == 0
               && geo->getSelectedTrips()->size() == reference(geo, start, end),
           QString("Delete region: nodes=%1 edges=%2").arg(graph->numberOfNodes()).arg(graph->numberOfEdges()));
    shot("09-delete");

    // --- Time stepping and the step size combo
    {
        auto combo = child<QComboBox>(times, "comboBox_2");
        auto fwd = child<QPushButton>(times, "stepForwardButton");
        auto back = child<QPushButton>(times, "stepBackButton");
        bool ok = true;
        QStringList seen;
        for (int index : {2, 0}) { // 1 hour, then 15 minutes
            combo->setCurrentIndex(index);
            const QDateTime s0 = times->getStartTime();
            fwd->click();
            settle(geo, view);
            const qint64 delta = s0.secsTo(times->getStartTime());
            seen << QString("%1 -> %2 s").arg(combo->currentText()).arg(delta);
            ok = ok && delta == (index == 2 ? 3600 : 900)
                    && geo->getSelectedTrips()->size() == reference(geo, times->getStartTime(), times->getEndTime());
            back->click();
            settle(geo, view);
            ok = ok && times->getStartTime() == s0;
        }
        report(ok && times->getStartTime() == start, "Time stepping / step size: " + seen.join(", "));
        combo->setCurrentIndex(2);
    }

    // --- Arrow keys step time from the map, key A stays a no-op without geometry shaders
    {
        geo->setFocus();
        QTest::keyClick(geo, Qt::Key_Right);
        settle(geo, view);
        const bool stepped = times->getStartTime() == start.addSecs(3600);
        QTest::keyClick(geo, Qt::Key_Left);
        settle(geo, view);
        report(stepped && times->getStartTime() == start, "Arrow keys step time from the map");

        QTest::keyClick(geo, Qt::Key_A);
        settle(geo, view);
        bool animation = false, locations = false;
        for (auto layer : geo->mapView()->getRenderingLayers()) {
            if (dynamic_cast<TripAnimation*>(layer)) animation = layer->isEnabled();
            if (dynamic_cast<TripLocationLOD*>(layer)) locations = layer->isEnabled();
        }
#ifdef Q_OS_MACOS
        report(!animation && locations, "Key A is a no-op on macOS (points stay visible)");
#else
        report(animation != locations, "Key A toggles between animation and points");
        QTest::keyClick(geo, Qt::Key_A);
#endif
        shot("10-key-a");
    }

    // --- Export (the button opens a native file dialog; drive the code behind it)
    {
        QTemporaryDir tmp;
        const QString path = tmp.filePath("export.csv");
        const size_t expected = geo->getSelectedTrips()->size();
        const bool started = view->exportToFile(path);
        QTest::qWaitFor([&] { return !view->exportBusy(); }, 60000);
        QFile csv(path);
        qint64 lines = 0;
        if (csv.open(QIODevice::ReadOnly)) while (!csv.readLine().isEmpty()) ++lines;
        report(started && lines == qint64(expected) + 1,
               QString("Export CSV: %1 rows for %2 trips").arg(lines - 1).arg(expected));
    }

    // --- Attribute exploration dialogs (Explore... button beside the map)
    {
        auto explore = child<QToolButton>(view, "exploreButton");
        tabs->setCurrentIndex(0);
        explore->click();
        TemporalSeriesDialog *ts = nullptr;
        QTest::qWaitFor([&] { return (ts = view->findChild<TemporalSeriesDialog*>()) || (ts = qobject_cast<TemporalSeriesDialog*>(QApplication::activeModalWidget())); }, 10000);
        if (!ts) for (auto w : QApplication::topLevelWidgets()) if (auto d = qobject_cast<TemporalSeriesDialog*>(w)) ts = d;
        if (ts) { QTest::qWaitFor([&] { return plotsIdle(ts); }, 60000); QTest::qWait(300); shot("11-temporal-exploration", ts); }
        report(ts && !ts->findChildren<QCustomPlot*>().isEmpty(),
               QString("Time-series attribute exploration dialog: %1 plots").arg(ts ? ts->findChildren<QCustomPlot*>().size() : 0));
        if (ts) { ts->reject(); QTest::qWait(100); }

        tabs->setCurrentIndex(1);
        explore->click();
        HistogramDialog *hd = nullptr;
        QTest::qWaitFor([&] {
            for (auto w : QApplication::topLevelWidgets()) if (auto d = qobject_cast<HistogramDialog*>(w)) hd = d;
            return hd != nullptr; }, 10000);
        if (hd) { QTest::qWaitFor([&] { return plotsIdle(hd); }, 60000); QTest::qWait(300); shot("12-histogram-exploration", hd); }
        report(hd && !hd->findChildren<QCustomPlot*>().isEmpty(),
               QString("Histogram attribute exploration dialog: %1 plots").arg(hd ? hd->findChildren<QCustomPlot*>().size() : 0));
        if (hd) { hd->reject(); QTest::qWait(100); }
        tabs->setCurrentIndex(0);
        settle(geo, view);
    }

    // --- Time exploration (Explore button in the time panel; accept the step-count prompt)
    {
        QTimer::singleShot(400, [] {
            if (auto d = qobject_cast<QInputDialog*>(QApplication::activeModalWidget())) d->accept();
        });
        child<QPushButton>(times, "exploreButton")->click(); // blocks in QInputDialog until the timer accepts
        QTest::qWaitFor([&] { return !view->explorationBusy(); }, 120000);
        TimeExplorationDialog *dialog = nullptr;
        QTest::qWaitFor([&] { return (dialog = view->findChild<TimeExplorationDialog*>()) != nullptr; }, 10000);
        int maps = 0;
        if (dialog) {
            QTest::qWaitFor([&] { return plotsIdle(dialog); }, 60000);
            QTest::qWait(1500); // tiles and GL viewports in every cell
            maps = dialog->findChildren<GeographicalViewWidget*>().size();
            shot("13-time-exploration", dialog);
        }
        report(dialog && maps == 2, QString("Time exploration dialog: %1 map cells (default 2 one-hour steps)").arg(maps));
        if (dialog) { dialog->close(); delete dialog; }
    }

    // --- Second map and the sync button
    {
        auto action = window.findChild<QAction*>("actionAddMap");
        if (action) action->trigger();
        QTest::qWait(500);
        auto views = window.findChildren<ViewWidget*>();
        bool ok = action && views.size() == 2;
        if (ok) {
            auto second = views[0] == view ? views[1] : views[0];
            auto geo2 = qobject_cast<GeographicalViewWidget*>(second->mapWidget());
            settle(geo2, second);
            child<QToolButton>(view, "syncButton")->click();
            child<QToolButton>(second, "syncButton")->click();
            QTest::qWait(200);
            // Zoom the first map with the wheel; the linked map must follow.
            const int zoom0 = geo->mapView()->zoomLevel();
            const QPoint c = geo->viewport()->rect().center();
            QWheelEvent wheel(c, geo->viewport()->mapToGlobal(c), QPoint(), QPoint(0, 120),
                              Qt::NoButton, Qt::NoModifier, Qt::NoScrollPhase, false);
            QApplication::sendEvent(geo->viewport(), &wheel);
            QTest::qWait(300);
            const bool zoomed = geo->mapView()->zoomLevel() != zoom0;
            const bool followed = geo2->mapView()->zoomLevel() == geo->mapView()->zoomLevel()
                && (geo2->mapView()->center() - geo->mapView()->center()).manhattanLength() < 1e-6;
            // Linked time stepping goes through the map's arrow keys (the step
            // buttons are per-view by design); each linked view advances one step.
            auto times2 = child<TimeSelectionWidget>(second, "timeSelectionWidget");
            const QDateTime s1 = times->getStartTime(), s2 = times2->getStartTime();
            geo->setFocus();
            QTest::keyClick(geo, Qt::Key_Right);
            settle(geo, view); settle(geo2, second);
            const bool timeFollowed = s1.secsTo(times->getStartTime()) == 3600
                                   && s2.secsTo(times2->getStartTime()) == 3600;
            report(zoomed && followed && timeFollowed,
                   QString("Second map + sync: zoom %1 -> %2, linked zoom %3, linked time %4")
                       .arg(zoom0).arg(geo->mapView()->zoomLevel()).arg(geo2->mapView()->zoomLevel())
                       .arg(timeFollowed ? "follows" : QString("DIVERGES (first +%1 s, linked +%2 s)")
                                .arg(s1.secsTo(times->getStartTime())).arg(s2.secsTo(times2->getStartTime()))));
            QTest::qWait(1500);
            shot("14-two-maps");
        } else report(false, "Second map: Views > New Map did not create a second view");
    }

    std::cout << (failures ? "FAILED: " : "All native interaction checks passed; ") << failures << " failure(s). Grabs in "
              << outDir.absolutePath().toStdString() << std::endl;
    return failures ? 1 : 0;
}
