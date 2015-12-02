#include "extendedplotwidget.h"
#include <QDebug>

QDateTime dateTimeFromTimet(time_t t){
    struct tm* st_tm =  localtime (&t);
    return QDateTime(QDate(st_tm->tm_year + 1900,
                           st_tm->tm_mon + 1,
                           st_tm->tm_mday),
                     QTime(st_tm->tm_hour,
                           st_tm->tm_min,
                           st_tm->tm_sec));
}

ExtendedPlotWidget::ExtendedPlotWidget(QWidget *parent) :
    QCustomPlot(parent),
    currentState(IDLE)
{
    this->setRangeDrag(Qt::Horizontal | Qt::Vertical);
    this->setRangeZoom(Qt::Horizontal | Qt::Vertical);
    this->setMouseTracking(true);
    buttonPressed = false;
}

void ExtendedPlotWidget::paintEvent(QPaintEvent *event){
    QCustomPlot::paintEvent(event);

    //
    QPainter painter(this);
    if(currentState == SELECTION){
        QPen pen;
        pen.setColor(Qt::gray);
        QColor color = Qt::gray;
        color.setAlphaF(0.3);
        painter.setPen(pen);
        painter.setBrush(color);

        qreal x,y,w,h;
        x = std::min(lastPoint.x(),currentPoint.x());
        y = 0;
        w = std::max(lastPoint.x(),currentPoint.x()) - x;
        h = height();

        //        //
        //        qDebug() << "Points " << lastPoint << " " << currentPoint << " "
        //                 << xAxis->pixelToCoord(x) << " " << xAxis->pixelToCoord(x+w);


        //        QDateTime start = QDateTime::fromMSecsSinceEpoch(xAxis->pixelToCoord(x)*1000 + 3600);
        //        QDateTime end = QDateTime::fromMSecsSinceEpoch(xAxis->pixelToCoord(x+w)*1000 + 3600);

        //        qDebug() << "Start " << start << " end " << end;

        painter.drawRect(QRectF(x,y,w,h));
    }

    if(underMouse()){
        QPen pen;
        pen.setColor(Qt::gray);
        QColor color = Qt::gray;
        color.setAlphaF(0.3);
        painter.setPen(pen);
        painter.setBrush(color);

        time_t t = xAxis->pixelToCoord(mousePosition.x());
        QDateTime mousePosTime = dateTimeFromTimet(t);

        painter.drawLine(mousePosition.x(),0,mousePosition.x(),height());

        int x = mousePosition.x()-180;
        if(x < 0)
            x += 190;
        painter.drawText(x,mousePosition.y(),180,20,0,mousePosTime.toString());
    }
}

void ExtendedPlotWidget::mousePressEvent(QMouseEvent *event){
    if(event->button() & Qt::RightButton){
        //draw selection
        lastPoint    = event->posF();
        currentPoint = lastPoint;
        currentState = SELECTION;
        repaint();
    }
    else{
        buttonPressed = true;
        QCustomPlot::mousePressEvent(event);
    }
}

void ExtendedPlotWidget::mouseMoveEvent(QMouseEvent *event){
    //
    mousePosition = event->posF();
    if(currentState == SELECTION){
        currentPoint = event->posF();
        repaint();
    }
    else if(buttonPressed){
        QCustomPlot::mouseMoveEvent(event);
    }
    else{
        repaint();
    }

}

void ExtendedPlotWidget::mouseReleaseEvent(QMouseEvent *event){
    if(currentState == SELECTION){
        currentState = IDLE;

        qreal x,y,w,h;
        x = std::min(lastPoint.x(),currentPoint.x());
        y = 0;
        w = std::max(lastPoint.x(),currentPoint.x()) - x;
        h = height();

        QDateTime start = dateTimeFromTimet(xAxis->pixelToCoord(x));
        QDateTime end = dateTimeFromTimet(xAxis->pixelToCoord(x+w));

        emit timeIntervalChanged(start,end);

        repaint();
    }
    else{
        QCustomPlot::mouseReleaseEvent(event);
    }

    buttonPressed = false;
}

void ExtendedPlotWidget::leaveEvent(QEvent *){
repaint();
}
