#include "coordinator.h"
#include "viewwidget.h"
#include "temporalseriesplotwidget.h"
#include "QMapWidget.hpp"
#include "geographicalviewwidget.h"
#include "util/colorbar.h"
#include "histogramwidget.h"

#include <QMutex>
#include <QtCore>
#include <QVector2D>

Coordinator* Coordinator::_instance = 0;

Coordinator::Coordinator(QObject *parent) :
    QObject(parent), enabled(true)
{
}

void Coordinator::addView(ViewWidget *view)
{
    _linkedViews.insert(view);
    _linkedPlots.insert(view->timeSeriesWidget());
    _linkedHists.insert(view->histogramWidget());
    this->addMapWidget((GeographicalViewWidget*)view->mapWidget());
    setSeriesAttribute(view->timeSeriesWidget()->plotAttribute());
    setHistsAttribute(view->histogramWidget()->plotAttribute());
}

void Coordinator::addMapWidget(GeographicalViewWidget *widget)
{
    _linkedMapViews.insert(widget->mapView());
    _linkedMapWidgets.insert(widget);
}

void Coordinator::removeView(ViewWidget *view)
{
    _linkedViews.remove(view);
    _linkedPlots.remove(view->timeSeriesWidget());
    _linkedHists.remove(view->histogramWidget());
    _linkedMapViews.remove(view->mapWidget()->mapView());
    _linkedMapWidgets.remove((GeographicalViewWidget*)view->mapWidget());

    view->timeSeriesWidget()->recomputePlots();
    view->histogramWidget()->recomputePlots();
}

bool Coordinator::containsView(ViewWidget *view)
{
    return this->enabled && _linkedViews.contains(view);
}

bool Coordinator::containsTimeSeries(TemporalSeriesPlotWidget *plot)
{
    return this->enabled && _linkedPlots.contains(plot);
}

bool Coordinator::containsMapView(QMapView *mw)
{
  return this->enabled && _linkedMapViews.contains(mw);
}

bool Coordinator::containsHist(HistogramWidget *plot)
{
  return this->enabled && _linkedHists.contains(plot);
}

void Coordinator::notifyAll()
{
    // create lines using attribute
    foreach (TemporalSeriesPlotWidget *plot,  _linkedPlots) {
        plot->setPlotAttribute(_seriesAttribute);
        plot->updatePlots();
    }

    // compute max and min
    _yMin =  1000000000.f;
    _yMax = -1000000000.f;
    foreach (TemporalSeriesPlotWidget *plot,  _linkedPlots) {
        _yMin = std::min(_yMin, plot->yMin());
        _yMax = std::max(_yMax, plot->yMax());
    }

    // update range in plots
    foreach (TemporalSeriesPlotWidget *plot,  _linkedPlots) {
        plot->updateYRange(_yMin, _yMax);
    }

    // compute scalar max and min
    QVector2D range(1000000000., -1000000000.);
    foreach (ViewWidget *view,  _linkedViews) {
      GeographicalViewWidget *gw = (GeographicalViewWidget*)view->mapWidget();
      QVector2D r = gw->getScalarRange();
      range.setX(std::min(range.x(), r.x()));
      range.setY(std::max(range.y(), r.y()));
    }
    foreach (ViewWidget *view,  _linkedViews) {
      GeographicalViewWidget *gw = (GeographicalViewWidget*)view->mapWidget();
      gw->getColorBar()->setRealMinMax(range.x(), range.y());
      gw->repaintContents();
    }

    // HISTOGRAMS
    foreach (HistogramWidget *plot,  _linkedHists) {
        plot->setPlotAttribute(_histsAttribute);
        plot->updatePlots();
    }

    // compute max and min for Histograms
    _yMin =  1000000000.f;
    _yMax = -1000000000.f;
    foreach (HistogramWidget *plot,  _linkedHists) {
        _yMin = std::min(_yMin, plot->yMin());
        _yMax = std::max(_yMax, plot->yMax());
    }
    // update range in plots
    foreach (HistogramWidget *plot,  _linkedHists) {
        plot->updateYRange(_yMin, _yMax);
    }

}

void Coordinator::notifyCameraChangeAll()
{
}

void Coordinator::stepBackward(int mins){

}

void Coordinator::stepForward(int mins){

}

Coordinator *Coordinator::instance()
{
    static QMutex mutex;
    if (!_instance) {
        mutex.lock();
        if (!_instance)
            _instance = new Coordinator;
        mutex.unlock();
    }
    return _instance;
}

bool Coordinator::isEnabled()
{
  return this->enabled;
}

void Coordinator::setEnabled(bool b)
{
  this->enabled = b;
}
