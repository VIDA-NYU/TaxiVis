// Regenerates the screenshots in doc/figs/usage and doc/figs/paper.
//
// Drives the real MainWindow with in-process Qt events (the same gestures the
// usage guide describes: Shift/Option + right-click drawing, the link tool, the
// step buttons) and saves window-system grabs, so the figures always show what
// the current build draws. Needs a display, OpenGL, the macOS screen-recording
// permission, map tiles (network or cache) and the full January 2013 index:
//
//   TZ=America/New_York TAXIVIS_DATA=~/data/FOIL2013/processed/2013_01.kdtrip \
//       build/src/TaxiVis/make_figures doc/figs
#include "mainwindow.h"
#include "viewwidget.h"
#include "geographicalviewwidget.h"
#include "QMapTileWidget.hpp"
#include "temporalseriesplotwidget.h"
#include "histogramwidget.h"
#include "scatterplotwidget.h"
#include "extendedplotwidget.h"
#include "timeselectionwidget.h"
#include "TimeExplorationDialog.hpp"
#include "TemporalSeriesDialog.hpp"
#include "HistogramDialog.hpp"
#include "global.h"
#include <QApplication>
#include <QComboBox>
#include <QDir>
#include <QInputDialog>
#include <QPushButton>
#include <QScreen>
#include <QSpinBox>
#include <QSplitter>
#include <QTabWidget>
#include <QTest>
#include <QTimer>
#include <QToolButton>
#include <iostream>

static QDir outDir;
static MainWindow *mainWin = nullptr;
static const int figureWidth = 1400; // pixels; window grabs are downscaled to this

template<class T> static T *child(QObject *root, const char *name) {
    T *w = root->findChild<T*>(name);
    if (!w) { std::cerr << "missing widget " << name << std::endl; std::exit(2); }
    return w;
}
static bool plotsIdle(QObject *root) {
    for (auto p : root->findChildren<TemporalSeriesPlotWidget*>()) if (p->computationBusy()) return false;
    for (auto p : root->findChildren<HistogramWidget*>()) if (p->computationBusy()) return false;
    for (auto p : root->findChildren<ScatterPlotWidget*>()) if (p->computationBusy()) return false;
    return true;
}
static QImage grabWindow(QWidget *window) {
    window->raise();
    window->activateWindow();
    QTest::qWait(400);
    const QImage image = window->screen()->grabWindow(window->winId()).toImage();
    if (image.isNull()) { std::cerr << "window grab refused (screen-recording permission?)" << std::endl; std::exit(3); }
    return image;
}
static void save(const QImage &image, const QString &name, int width = figureWidth) {
    const QString path = outDir.filePath(name);
    QDir().mkpath(QFileInfo(path).absolutePath());
    (image.width() > width ? image.scaledToWidth(width, Qt::SmoothTransformation) : image).save(path);
    std::cout << "wrote " << path.toStdString() << std::endl;
}
// Drop the hover read-out if the real pointer happens to rest on a plot.
static void clearHover(QWidget *root) {
    for (auto plot : root->findChildren<ExtendedPlotWidget*>()) { QEvent leave(QEvent::Leave); QApplication::sendEvent(plot, &leave); }
    QTest::qWait(100);
}
// Crop `part` (a descendant of `window`) out of a grab of that window.
static QImage crop(const QImage &grab, QWidget *window, QWidget *part, QRect rect = QRect()) {
    const double scale = double(grab.width()) / window->width();
    if (rect.isNull()) rect = part->rect();
    const QPoint origin = part->mapTo(window, rect.topLeft());
    return grab.copy(QRect(origin * scale, rect.size() * scale));
}

// One map view and the gestures a user would make on it.
struct Driver {
    ViewWidget *view;
    GeographicalViewWidget *geo;
    QWidget *vp;
    TimeSelectionWidget *times;
    QTabWidget *tabs = nullptr;

    explicit Driver(ViewWidget *v) : view(v) {
        geo = qobject_cast<GeographicalViewWidget*>(view->mapWidget());
        vp = geo->viewport();
        times = child<TimeSelectionWidget>(view, "timeSelectionWidget");
        for (auto t : view->findChildren<QTabWidget*>()) if (t->indexOf(view->histogramWidget()) >= 0) tabs = t;
    }
    void settle(int tileWait = 0) {
        QTest::qWaitFor([&] { return !geo->queryBusy() && plotsIdle(view); }, 120000);
        QTest::qWait(250 + tileWait);
        geo->repaintContents();
        QTest::qWait(150);
    }
    QPoint at(double lat, double lon) { return geo->mapView()->mapFromGeoLocation(QPointF(lat, lon)).toPoint(); }
    void lookAt(double lat, double lon, int zoom) {
        geo->mapView()->setView(QPointF(lat, lon), zoom);
        settle(3500); // tiles
    }
    void query(QDateTime start, QDateTime end, int stepIndex) {
        child<QComboBox>(times, "comboBox_2")->setCurrentIndex(stepIndex); // 15 min, 30 min, 1 hour, 1 day, 1 week
        times->setTimes(start, end);
        child<QPushButton>(times, "queryButton")->click();
        settle();
    }
    void tool(const char *button) { child<QToolButton>(view, button)->click(); }
    void rectangle(double lat0, double lon0, double lat1, double lon1) {
        const QPoint a = at(lat0, lon0), b = at(lat1, lon1);
        QTest::mousePress(vp, Qt::RightButton, Qt::ShiftModifier, a);
        for (int s = 1; s <= 8; ++s) QTest::mouseMove(vp, a + (b - a) * s / 8, 2);
        QTest::mouseRelease(vp, Qt::RightButton, Qt::ShiftModifier, b);
        settle();
    }
    void polygon(const QList<QPointF> &latlon) {
        QList<QPoint> pts;
        for (auto p : latlon) pts << at(p.x(), p.y());
        // QTest::mouseMove does not reach the view when no button is held, but the
        // rubber-band vertex only follows real move events, so send them directly.
        auto hover = [&](QPoint p) {
            QMouseEvent move(QEvent::MouseMove, QPointF(p), QPointF(vp->mapToGlobal(p)), Qt::NoButton, Qt::NoButton, Qt::NoModifier);
            QApplication::sendEvent(vp, &move);
        };
        QTest::mouseClick(vp, Qt::RightButton, Qt::AltModifier, pts.first());
        for (int i = 1; i < pts.size(); ++i) {
            hover(pts[i]);
            QTest::mouseClick(vp, Qt::LeftButton, Qt::NoModifier, pts[i]);
        }
        hover(pts.first());
        QTest::mouseClick(vp, Qt::LeftButton, Qt::NoModifier, pts.first()); // close on the first vertex
        settle();
        // Guard against silently mis-drawn regions: the newest node must have every vertex.
        SelectionGraph::NodeReverseIterator it, end;
        geo->getSelectionGraph()->getNodeReverseIterator(it, end);
        const int elements = it == end ? 0 : it->second->getSelection()->getGeometry().elementCount();
        if (elements != pts.size()) {
            std::cerr << "polygon drawn with " << elements << " of " << pts.size() << " vertices; screen points:";
            for (auto p : pts) std::cerr << " (" << p.x() << "," << p.y() << ")";
            std::cerr << std::endl;
            std::exit(4);
        }
    }
    void link(QPointF from, QPointF to) {
        const QPoint a = at(from.x(), from.y()), b = at(to.x(), to.y());
        QTest::mousePress(vp, Qt::RightButton, Qt::NoModifier, a);
        for (int s = 1; s <= 8; ++s) QTest::mouseMove(vp, a + (b - a) * s / 8, 2);
        QTest::mouseRelease(vp, Qt::RightButton, Qt::NoModifier, b);
        settle();
    }
    void clearRegions() {
        SelectionGraph empty;
        geo->updateSelectionGraph(&empty);
        settle();
    }
    void typeInto(QSpinBox *spin, int value) {
        spin->setFocus();
        spin->selectAll();
        QTest::keyClicks(spin, QString::number(value));
        QTest::keyClick(spin, Qt::Key_Return);
        settle();
    }
    void choose(QComboBox *combo, const QString &text) {
        const int index = combo->findText(text, Qt::MatchContains);
        if (index < 0) {
            std::cerr << "no combo entry containing '" << text.toStdString() << "'; entries:";
            for (int i = 0; i < combo->count(); ++i) std::cerr << " [" << combo->itemText(i).toStdString() << "]";
            std::cerr << std::endl;
            std::exit(2);
        }
        combo->setCurrentIndex(index);
        settle();
    }
    void shot(const QString &name) {
        vp->setFocus();
        settle();
        clearHover(view);
        save(grabWindow(mainWin), name);
    }
};

static QDate jan(int day) { return QDate(2013, 1, day); }
static QDateTime at(QDate d, int hour = 0) { return QDateTime(d, QTime(hour, 0)); }

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    outDir = QDir(argc > 1 ? argv[1] : "doc/figs");
    Global::getInstance();

    MainWindow window;
    mainWin = &window;
    window.show();
    if (!QTest::qWaitForWindowExposed(&window)) { std::cerr << "window not exposed\n"; return 2; }
    Driver d(window.findChild<ViewWidget*>());
    auto splitter = child<QSplitter>(d.view, "splitter");
    const QList<int> defaultSplit = splitter->sizes();
    const double midtownLat = 40.7560, midtownLon = -73.9830;

    // ---------------------------------------------------------------- usage guide
    d.lookAt(midtownLat, midtownLon, 15);
    d.query(at(jan(1)), at(jan(2)), 2);
    d.shot("usage/01_overview.png");
    save(grabWindow(&window), "taxivis.png", 1855);
    {
        auto toolbar = child<QToolButton>(d.view, "pickupModeButton")->parentWidget();
        save(crop(grabWindow(&window), &window, toolbar), "usage/09_toolbar.png", 110);
    }

    child<QPushButton>(d.times, "stepForwardButton")->click();
    d.shot("usage/08_step_time.png");
    child<QPushButton>(d.times, "stepBackButton")->click();
    d.settle();

    d.tool("pickupModeButton");
    d.rectangle(40.7612, -73.9895, 40.7575, -73.9835); // Theater District
    d.shot("usage/02_pickup_rectangle.png");

    d.tool("dropoffModeButton");
    const QList<QPointF> grandCentral = {{40.7545, -73.9800}, {40.7552, -73.9755}, {40.7512, -73.9742}, {40.7506, -73.9785}};
    d.polygon(grandCentral);
    d.shot("usage/03_pickup_and_dropoff.png");

    d.tool("linkModeButton");
    d.link({40.7592, -73.9862}, {40.7530, -73.9770});
    d.shot("usage/04_linked_regions.png");
    d.tabs->setCurrentIndex(0);
    d.shot("usage/05_time_series.png");

    d.tabs->setCurrentIndex(1);
    d.choose(child<QComboBox>(d.view->histogramWidget(), "attributeComboBox"), "Fare");
    d.shot("usage/06_histogram.png");
    d.tabs->setCurrentIndex(2);
    d.choose(child<QComboBox>(d.tabs->widget(2), "comboBox"), "Hour");
    d.choose(child<QComboBox>(d.tabs->widget(2), "comboBox_2"), "Duration");
    d.shot("usage/07_scatter.png");

    // Attribute exploration for the linked query: one small plot per attribute.
    d.tabs->setCurrentIndex(0);
    d.settle();
    d.tool("exploreButton");
    {
        TemporalSeriesDialog *dialog = nullptr;
        QTest::qWaitFor([&] {
            for (auto w : QApplication::topLevelWidgets()) if (auto t = qobject_cast<TemporalSeriesDialog*>(w)) dialog = t;
            return dialog != nullptr; }, 10000);
        if (!dialog) { std::cerr << "attribute exploration dialog missing" << std::endl; return 2; }
        QTest::qWaitFor([&] { return plotsIdle(dialog); }, 60000);
        QTest::qWait(800);
        clearHover(dialog);
        save(grabWindow(dialog), "paper/attribute_exploration.png");
        dialog->reject();
        QTest::qWait(300);
    }

    // ---------------------------------------------------------------- paper figures
    // Fig. 2: hourly point clouds over Manhattan, every trip drawn (LOD off).
    d.tool("pickupAndDropoffButton");
    d.clearRegions();
    d.tabs->setCurrentIndex(0);
    splitter->setSizes({10000, 0}); // give the map the whole height for the portrait crops
    QTest::qWait(300);
    d.lookAt(40.7740, -73.9650, 12);
    QTest::keyClick(d.geo, Qt::Key_L);
    for (int hour = 7; hour <= 10; ++hour) {
        d.query(at(jan(2), hour), at(jan(2), hour + 1), 2);
        d.vp->setFocus();
        d.settle(500);
        const int w = qMin(d.vp->width(), int(d.vp->height() * 0.68));
        const QRect portrait((d.vp->width() - w) / 2, 0, w, d.vp->height());
        save(crop(grabWindow(&window), &window, d.vp, portrait),
             QString("paper/manhattan_jan02_%1h.png").arg(hour, 2, 10, QChar('0')), 640);
    }
    QTest::keyClick(d.geo, Qt::Key_L);
    splitter->setSizes(defaultSplit);
    QTest::qWait(300);

    // Fig. 7: heat map of January 1st.
    d.lookAt(40.7500, -73.9500, 11);
    d.query(at(jan(1)), at(jan(2)), 3);
    QTest::keyClick(d.geo, Qt::Key_1);
    d.shot("paper/heatmap_jan01.png");

    // Fig. 11: time exploration, two one-day steps, with the heat map still on.
    {
        QTimer::singleShot(500, [] {
            if (auto input = qobject_cast<QInputDialog*>(QApplication::activeModalWidget())) {
                input->setIntValue(2);
                input->accept();
            }
        });
        child<QPushButton>(d.times, "exploreButton")->click();
        QTest::qWaitFor([&] { return !d.view->explorationBusy(); }, 300000);
        TimeExplorationDialog *dialog = nullptr;
        QTest::qWaitFor([&] { return (dialog = d.view->findChild<TimeExplorationDialog*>()) != nullptr; }, 10000);
        if (!dialog) { std::cerr << "time exploration dialog missing" << std::endl; return 2; }
        QTest::qWaitFor([&] { return plotsIdle(dialog); }, 120000);
        // The dialog opens with short map cells that clip the colour bar; drag
        // its splitter to an even split, as a user would.
        if (auto split = dialog->findChild<QSplitter*>("splitter")) split->setSizes({420, 380});
        QTest::qWait(5000); // tiles and heat maps in every cell
        clearHover(dialog);
        save(grabWindow(dialog), "paper/time_exploration.png");
        dialog->close();
        delete dialog;
    }
    QTest::keyClick(d.geo, Qt::Key_1);

    // Fig. 8: four pickup neighbourhoods over a week, 168 hourly bins. Drawn
    // uptown to downtown so the colour groups match the text in figures.md.
    d.lookAt(40.7600, -73.9700, 12);
    d.tool("pickupModeButton");
    d.query(at(jan(1)), at(jan(8)), 2);
    d.polygon({{40.8040, -73.9712}, {40.8170, -73.9620}, {40.8090, -73.9345}, {40.7925, -73.9375}}); // Harlem and upper Manhattan
    d.polygon({{40.7645, -73.9730}, {40.7880, -73.9555}, {40.7815, -73.9420}, {40.7585, -73.9585}}); // Upper East Side
    d.polygon({{40.7420, -74.0090}, {40.7720, -73.9940}, {40.7590, -73.9590}, {40.7290, -73.9725}}); // Midtown
    d.polygon({{40.7005, -74.0190}, {40.7400, -74.0100}, {40.7280, -73.9720}, {40.7075, -73.9995}}); // Lower Manhattan
    d.typeInto(child<QSpinBox>(d.view->timeSeriesWidget(), "numBinsSpin"), 168);
    d.shot("paper/neighborhoods_week.png");
    d.typeInto(child<QSpinBox>(d.view->timeSeriesWidget(), "numBinsSpin"), 10);

    // Fig. 1: Lower Manhattan to the airports.
    d.clearRegions();
    d.lookAt(40.7100, -73.9000, 11);
    d.query(at(jan(1)), at(jan(2)), 3);
    d.tool("pickupModeButton");
    d.polygon({{40.6990, -74.0220}, {40.7290, -74.0150}, {40.7230, -73.9720}, {40.7030, -73.9900}}); // Lower Manhattan
    d.tool("dropoffModeButton");
    d.rectangle(40.7830, -73.8900, 40.7640, -73.8560); // LaGuardia
    d.rectangle(40.6660, -73.8250, 40.6350, -73.7650); // JFK
    d.tool("linkModeButton");
    d.link({40.7130, -74.0050}, {40.7735, -73.8730});
    d.link({40.7130, -74.0050}, {40.6500, -73.7950});
    d.tabs->setCurrentIndex(0);
    d.shot("paper/airports_link_colours.png");

    d.tabs->setCurrentIndex(2);
    d.query(at(jan(6)), at(jan(7)), 3);
    d.shot("paper/airports_sunday.png");
    child<QPushButton>(d.times, "stepForwardButton")->click();
    d.shot("paper/airports_monday.png");

    std::cout << "done" << std::endl;
    return 0;
}
