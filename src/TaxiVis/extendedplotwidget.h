#ifndef EXTENDEDPLOTWIDGET_H
#define EXTENDEDPLOTWIDGET_H

#include "qcustomplot.h"

class ExtendedPlotWidget : public QCustomPlot
{
    Q_OBJECT
public:
    enum State {IDLE,SELECTION};
public:
    explicit ExtendedPlotWidget(QWidget *parent = 0);
private:
    State   currentState;
    QPointF lastPoint;
    QPointF currentPoint;
    QPointF mousePosition;
    bool    buttonPressed;
signals:
    void timeIntervalChanged(QDateTime,QDateTime);
public slots:
protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void leaveEvent(QEvent *);

};

#endif // EXTENDEDPLOTWIDGET_H
