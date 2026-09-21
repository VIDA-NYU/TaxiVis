#include "TemporalSeriesDialog.hpp"
#include "ui_TemporalSeriesDialog.h"
#include "geographicalviewwidget.h"
#include "global.h"

TemporalSeriesDialog::TemporalSeriesDialog(GeographicalViewWidget *geo, int numBins)
    : QDialog(geo),
      ui(new Ui::TemporalSeriesDialog),
      geoWidget(geo)
{
    this->ui->setupUi(this);


    tripSnapshot=*geoWidget->getSelectedTrips();
    graphSnapshot.assign(geoWidget->getSelectionGraph());
    KdTrip::TripSet *trips = &tripSnapshot;
    QDateTime startTime = this->geoWidget->getSelectedStartTime();
    QDateTime endTime = this->geoWidget->getSelectedEndTime();
    SelectionGraph *selectionGraph = &graphSnapshot;
    QGridLayout *layout = this->ui->gridLayout;

    //add extra widgets to deal with the extra fields
    Global* global = Global::getInstance();
    int numExtraFields = global->numExtraFields();
    int numCols = layout->columnCount();
    int currentIndex = 0;
    for (int i=0; i<layout->count(); i++) {
        TemporalSeriesPlotWidget *widget = dynamic_cast<TemporalSeriesPlotWidget*>(layout->itemAt(i)->widget());
        if (widget) {
            ++currentIndex;
        }
    }

    //
    for(int i = 0 ; i < numExtraFields ; ++i){
        ExtraField field = global->getExtraField(i);
        if(field.active){
            TemporalSeriesPlotWidget *widget = new TemporalSeriesPlotWidget(this);
            int col = currentIndex % numCols;
            int row = currentIndex / numCols;

            layout->addWidget(widget,row,col,1,1);
            ++currentIndex;
        }
    }

    //
    for (int i=0; i<layout->count(); i++) {
        TemporalSeriesPlotWidget *widget = dynamic_cast<TemporalSeriesPlotWidget*>(layout->itemAt(i)->widget());
        TemporalSeriesPlotWidget::PlotAttribute currentAttribute = (TemporalSeriesPlotWidget::PlotAttribute)i;

        if (widget) {
            QObjectList children = widget->children();
            for (int j=0; j<children.count(); j++){
                if (QWidget *w = dynamic_cast<QWidget*>(children[j])) {
                    ExtendedPlotWidget *plot = dynamic_cast<ExtendedPlotWidget*>(w);

                    //
                    if (!plot)
                        w->hide();
                    else
                        this->plots.push_back(plot);
                }
            }

            widget->setDateTimes(startTime, endTime);
            widget->setSelectionGraph(selectionGraph);
            widget->setSelectedTripsRepository(trips);
            widget->setPlotAttribute(currentAttribute);
            widget->setNumberOfBins(numBins);
        }
    }
    for (auto plot : plots) plot->xAxis->setRange(startTime.toSecsSinceEpoch(),endTime.toSecsSinceEpoch());
    for (int i=0; i<this->plots.count(); i++) {
        this->connect(this->plots[i]->axisRect()->rangeDragAxis(Qt::Horizontal),
                      SIGNAL(rangeChanged(const QCPRange&)),
                      this, SLOT(xAxisRangeChanged(const QCPRange&)));
        this->connect(this->plots[i],
                      SIGNAL(timeIntervalChanged(QDateTime,QDateTime)),
                      this, SLOT(timeIntervalChanged(QDateTime,QDateTime)));
    }
}

TemporalSeriesDialog::~TemporalSeriesDialog()
{
    for (auto widget : findChildren<TemporalSeriesPlotWidget*>()) delete widget;
    delete this->ui;
}

void TemporalSeriesDialog::xAxisRangeChanged(const QCPRange &newRange)
{
    for (int i=0; i<this->plots.count(); i++)
        this->disconnect(this->plots[i]->axisRect()->rangeDragAxis(Qt::Horizontal),
                         SIGNAL(rangeChanged(const QCPRange&)),
                         this, SLOT(xAxisRangeChanged(const QCPRange&)));
    for (int i=0; i<this->plots.count(); i++) {
        this->plots[i]->axisRect()->rangeDragAxis(Qt::Horizontal)->setRange(newRange);
        this->plots[i]->replot();
    }
    for (int i=0; i<this->plots.count(); i++)
        this->connect(this->plots[i]->axisRect()->rangeDragAxis(Qt::Horizontal),
                      SIGNAL(rangeChanged(const QCPRange&)),
                      this, SLOT(xAxisRangeChanged(const QCPRange&)));
}


void TemporalSeriesDialog::timeIntervalChanged(QDateTime start, QDateTime end)
{
    for (int i=0; i<this->plots.count(); i++) {
        this->plots[i]->axisRect()->rangeDragAxis(Qt::Horizontal)->setRange(start.toSecsSinceEpoch(), end.toSecsSinceEpoch());
        this->plots[i]->replot();
    }
}

QDateTime TemporalSeriesDialog::startTime()
{
    time_t t = this->plots[0]->axisRect()->rangeDragAxis(Qt::Horizontal)->range().lower;
    struct tm* st_tm =  localtime (&t);
    return QDateTime(QDate(st_tm->tm_year + 1900,
                           st_tm->tm_mon + 1,
                           st_tm->tm_mday),
                     QTime(st_tm->tm_hour,
                           st_tm->tm_min,
                           st_tm->tm_sec));
}

QDateTime TemporalSeriesDialog::endTime()
{
    time_t t = this->plots[0]->axisRect()->rangeDragAxis(Qt::Horizontal)->range().upper;
    struct tm* st_tm =  localtime (&t);
    return QDateTime(QDate(st_tm->tm_year + 1900,
                           st_tm->tm_mon + 1,
                           st_tm->tm_mday),
                     QTime(st_tm->tm_hour,
                           st_tm->tm_min,
                           st_tm->tm_sec));

}
