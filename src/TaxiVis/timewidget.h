#ifndef TIMEWIDGET_H
#define TIMEWIDGET_H

#include <QWidget>
#include <QDateTime>
#include <vector>
#include <iostream>
#include <string>

namespace Ui {
class TimeWidget;
}

typedef QPair<QDateTime,QDateTime> DateTimePair;
typedef QList<DateTimePair> DateTimeList;

enum TimeType {
    DAYS = 0,
    HOURS,
    MONTHS,
    YEARS,
    INVALID
};

class CellsWidget: public QWidget
{
    Q_OBJECT

public:
    explicit CellsWidget( QWidget *parent = 0 );
    ~CellsWidget();

    void setType( TimeType type );
    TimeType type() const { return _type; }
    void changeCurrentSelection(int id) { _selecteds.push_back(id); }
    void getSelectedCells(bool *cells);

signals:
    void currentCellChanged( const int, const TimeType );

protected:
    void paintEvent( QPaintEvent * );
    void mousePressEvent ( QMouseEvent * e );
    void mouseReleaseEvent ( QMouseEvent * e );

private:
    TimeType         _type;
    std::vector<int> _selecteds;
    std::string     *_strs;
    int              _numCells;
    QPoint           _lastPoint;
    bool             _mouseClick;
};

class TimeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TimeWidget( QWidget *parent = 0 );
    ~TimeWidget();
    void getSelectedCells(bool *years, bool *months, bool *days, bool *hours);
  
    DateTimeList getSelectedRanges();
    QList< QDate > getSelectedDays();
    QList< QPair<QTime,QTime> > getSelectedHours();
  
  
signals:
    void timeSelection( const int index, const TimeType type );

private slots:
    void selectionChanged( const int index, const TimeType type );

private:
    Ui::TimeWidget *_ui;
};

#endif // TIMEWIDGET_H
