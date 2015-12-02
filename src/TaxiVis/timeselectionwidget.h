#ifndef TIMESELECTIONWIDGET_H
#define TIMESELECTIONWIDGET_H

#include <QWidget>
#include <QDateTime>
#include "timewidget.h"

namespace Ui {
class TimeSelectionWidget;
}

class TimeSelectionWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit TimeSelectionWidget(QWidget *parent = 0);
    ~TimeSelectionWidget();
    
private:
    Ui::TimeSelectionWidget *ui;
    int getStepSize();

public:
    QDateTime getStartTime();
    QDateTime getEndTime();
    void      setTimes(QDateTime start,QDateTime end);
    void      setStepSize(int mins);

signals:
    void timeUpdated(QDateTime,QDateTime);
    void recurrentTimeUpdated(TimeWidget *widget);
    void exploreInTime(const DateTimeList &timeRange);

private slots:

public slots:
    void stepBack();
    void stepForward();
    void timeDialogsChanged();
    void exploreTime();
};

#endif // TIMESELECTIONWIDGET_H
