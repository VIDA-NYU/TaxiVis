#ifndef COORDINATOR_H
#define COORDINATOR_H

#include "temporalseriesplotwidget.h"
#include "histogramwidget.h"

#include <QObject>
#include <QSet>

class ViewWidget;
class QMapView;
class GeographicalViewWidget;

class Coordinator : public QObject
{
    Q_OBJECT
public:
    explicit Coordinator(QObject *parent = 0);
    static Coordinator* instance();

    void addView(ViewWidget *view);
    void addMapWidget(GeographicalViewWidget *widget);

    void removeView(ViewWidget *view);
    bool containsView(ViewWidget *view);
    bool containsTimeSeries(TemporalSeriesPlotWidget *plot);
    bool containsHist(HistogramWidget *plot);
    bool containsMapView(QMapView *mw);

    void notifyAll();
    void notifyCameraChangeAll();

    // set share variables
    void setSeriesAttribute(TemporalSeriesPlotWidget::PlotAttribute attribute) { _seriesAttribute=attribute; }
    void setHistsAttribute(HistogramWidget::PlotAttribute attribute) { _histsAttribute=attribute; }
    void setMapCenter(QPointF mapCenter) { _mapCenter = mapCenter; }
    void setMapLevel(int mapLevel) { _mapLevel = mapLevel; }
    //
    void stepBackward(int mins);
    void stepForward(int mins);

    QSet<QMapView*> linkedMapViews() { return _linkedMapViews; }
    QSet<GeographicalViewWidget*> linkedMapWidgets() { return _linkedMapWidgets; }

    bool isEnabled();
    void setEnabled(bool b);

private:

    bool enabled;

    QSet<ViewWidget *> _linkedViews;
    QSet<TemporalSeriesPlotWidget *> _linkedPlots;
    QSet<QMapView *> _linkedMapViews;
    QSet<GeographicalViewWidget *> _linkedMapWidgets;
    QSet<HistogramWidget *> _linkedHists;

    // For Vertical Range
    float _yMin;
    float _yMax;

    // For Plot Attribute
    TemporalSeriesPlotWidget::PlotAttribute _seriesAttribute;
    HistogramWidget::PlotAttribute          _histsAttribute;

    // For Camera Information
    QPointF _mapCenter;
    int     _mapLevel;

    static Coordinator* _instance;
};

#endif // COORDINATOR_H
