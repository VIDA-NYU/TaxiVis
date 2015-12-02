#ifndef EXTENDEDHISTOGRAM_H
#define EXTENDEDHISTOGRAM_H

#include "qcustomplot.h"
#include <QList>
#include <QInputDialog>

typedef std::pair<float,float> IntervalSelection;

class ExtendedHistogram : public QCustomPlot
{
    Q_OBJECT
public:
    enum State {IDLE,SELECTION,DRAG,SCALE};
public:
    ExtendedHistogram(QWidget *parent = 0);
    void addSelection(double selMin,double selMax);
    void setControlValues(double x0, double convertedValue0,
                          double x1, double convertedValue1);
    QList<IntervalSelection> getSelections();
    void clearSelections();
protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
private:
    QList<IntervalSelection> selections;
    State                    currentState;
    QPointF                  basePoint;
    QPointF                  lastPoint;
    QPointF                  currentPoint;
    int                      currentSelectionIndex;
    double x0;
    double convertedValue0;
    double x1;
    double convertedValue1;

    void pickSelection(QPointF p, bool&, int &index);
    void convertValue(const double x, double& value);
    void convertSelections(QList<IntervalSelection>&);
    void convertSelection(const double &lowerBound, const double &upperBound,
                          IntervalSelection&);
    void inverseConvertSelection(const double &lowerBound, const double &upperBound, IntervalSelection &sel);
    void inverseConvertValue(const double x, double &value);

signals:
    void updateSelection(QList<IntervalSelection>);
};

#endif // EXTENDEDHISTOGRAM_H
