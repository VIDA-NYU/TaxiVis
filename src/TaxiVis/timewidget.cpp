#include "timewidget.h"
#include "ui_timewidget.h"

#include <QtGui>
#include "global.h"

const std::string days[] = { "Mon.", "Tue.", "Wed.", "Thu.", "Fri.", "Sat.", "Sun."};
const int numDays = 7;
const std::string hours[] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10",
                              "11", "12", "13", "14", "15", "16", "17", "18", "19",
                              "20", "21", "22", "23" };
const int numHours = 24;
const std::string months[] = { "Jan.", "Feb.", "Mar.", "Apr.", "May.", "June.",
                               "July.", "Aug.", "Sept.", "Oct.", "Nov.", "Dec." };
const int numMonths = 12;

int numYears = 4;
std::string* years;
int* years_int;

CellsWidget::CellsWidget( QWidget *parent ) :
    QWidget( parent ), _mouseClick(false)
{
    setType( DAYS );
}

CellsWidget::~CellsWidget()
{}

void CellsWidget::setType( TimeType type )
{
    _type = type;
    if ( _type == DAYS ) {
        _strs  = const_cast< std::string * >( days );
        _numCells = numDays;
    } else if ( _type == HOURS ) {
        _strs  = const_cast< std::string * >( hours );
        _numCells = numHours;
    } else if ( _type == MONTHS ) {
        _strs  = const_cast< std::string * >( months );
        _numCells = numMonths;
    } else if ( _type == YEARS ) {
        _strs  = const_cast< std::string * >( years );
        _numCells = numYears;
    }

    update();
}

void CellsWidget::paintEvent( QPaintEvent *event )
{
    QPainter painter( this );
    painter.setRenderHint( QPainter::Antialiasing );
    painter.setPen( Qt::darkGreen );
    painter.setPen( Qt::black );
    painter.setBrush( Qt::white );

    QRect grect = geometry();
    int	step  = grect.width() / _numCells;
    for ( int i = 0; i < _numCells; ++i ) {
        int width = i==_numCells-1?grect.width()-i*step:step;
        QRect rect( i * step, 0, width, grect.height() );
        painter.drawRect( rect );
        painter.drawText( rect, Qt::AlignCenter, _strs[i].c_str() );
    }

    painter.setBrush( Qt::gray );
    for ( int i = 0; i < _selecteds.size(); ++i ) {
        int width = _selecteds[i]==_numCells-1?grect.width()-_selecteds[i]*step:step;
        QRect rect( _selecteds[i] * step, 0, width, grect.height() );
        painter.drawRect( rect );
        painter.drawText( rect, Qt::AlignCenter, _strs[_selecteds[i]].c_str() );
    }
}

void CellsWidget::mousePressEvent(QMouseEvent *e) {
    _lastPoint = e->pos();
    _mouseClick = true;
}

void CellsWidget::mouseReleaseEvent(QMouseEvent *e) {
    if ((_mouseClick) && (e->pos() == _lastPoint)) {
        if ( e->button() == Qt::LeftButton ) {
            int step  = width() / _numCells;
            int index = 0;
            for ( int i = 1; i <= _numCells; ++i, index++ )
                if ( e->pos().x() < i * step )
                    break;
            _selecteds.push_back( index );
            emit currentCellChanged( index, _type );
        } else if ( e->button() == Qt::RightButton ) {
            _selecteds.clear();
            emit currentCellChanged( -1, _type );
        }
        update();
    }
}

void CellsWidget::getSelectedCells(bool *cells)
{
  memset(cells, 0, sizeof(bool)*this->_numCells);
  for (size_t i=0; i<this->_selecteds.size(); i++)
    cells[this->_selecteds[i]] = true;
}

TimeWidget::TimeWidget( QWidget *parent ) :
    QWidget( parent ),
    _ui( new Ui::TimeWidget )
{
    //init years info
    Global* global = Global::getInstance();
    QVector<int> dataYears;
    global->getDataYears(dataYears);
    numYears = dataYears.size();
    years_int = new int[numYears];
    years     = new std::string[numYears];
    for(int i = 0 ; i < numYears ; ++i){
        years_int[i] = dataYears[i];
        years[i] = QString::number(dataYears[i]).toStdString();
    }

    //
    _ui->setupUi( this );
    _ui->widgetYears->setType( YEARS );
    _ui->widgetMonths->setType( MONTHS );
    _ui->widgetDays->setType( DAYS );
    _ui->widgetHours->setType( HOURS );

    // Forcing to select
    _ui->widgetYears->changeCurrentSelection(1);
    _ui->widgetMonths->changeCurrentSelection(4);
    _ui->widgetDays->changeCurrentSelection(1);

    connect( _ui->widgetMonths, SIGNAL( currentCellChanged( int, TimeType ) ),
             this, SLOT( selectionChanged( int, TimeType ) ) );
    connect( _ui->widgetDays, SIGNAL( currentCellChanged( int, TimeType ) ),
             this, SLOT( selectionChanged( int, TimeType ) ) );
    connect( _ui->widgetHours, SIGNAL( currentCellChanged( int, TimeType ) ),
             this, SLOT( selectionChanged( int, TimeType ) ) );
}

TimeWidget::~TimeWidget()
{    
    delete _ui;
}

void TimeWidget::selectionChanged( const int index, const TimeType type )
{
    emit timeSelection( index, type );
}

void TimeWidget::getSelectedCells(bool *years, bool *months, bool *days, bool *hours)
{
  this->_ui->widgetYears->getSelectedCells(years);
  this->_ui->widgetMonths->getSelectedCells(months);
  this->_ui->widgetDays->getSelectedCells(days);
  this->_ui->widgetHours->getSelectedCells(hours);
}


QList< QPair<QTime,QTime> > TimeWidget::getSelectedHours()
{
  bool hours[24];
  this->_ui->widgetHours->getSelectedCells(hours);
  QList< QPair<QTime,QTime> > hRange;
  int b, e;
  b = e = -1;
  for (int h=0; h<25; h++) {
    if (h<24 && hours[h]) {
      if (b<0)
        b = e = h;
      else
        e = h;
    }
    else if (e>=0) {
      hRange.append(qMakePair(QTime(b, 0, 0), QTime(e, 59, 59)));
      b = e = -1;
    }
  }
  if (hRange.size()==0)
    hRange.append(qMakePair(QTime(0, 0, 0), QTime(23, 59, 59)));
  return hRange;
}

QList<QDate> TimeWidget::getSelectedDays()
{
  bool yearFlags[numYears], months[12], days[7];
  this->_ui->widgetYears->getSelectedCells(yearFlags);
  this->_ui->widgetMonths->getSelectedCells(months);
  this->_ui->widgetDays->getSelectedCells(days);
  bool hasDay = false;
  for (int i=0; i<7; i++)
    if (days[i]) {
      hasDay = true;
      break;
    }
  if (!hasDay)
    std::fill_n(days, 7, true);
  QList<QDate> dRange;
  //  
  for (int y=0; y<numYears; y++)
    if (yearFlags[y])
      for (int m=0; m<12; m++)
        if (months[m]) {
          QDate now(years_int[y], m+1, 1);
          while (now.month()==m+1) {
            if (days[now.dayOfWeek()-1])
              dRange.push_back(now);
            now = now.addDays(1);
          }
        }
  return dRange;
}

DateTimeList TimeWidget::getSelectedRanges()
{
  QList< QPair<QTime,QTime> > hRange = this->getSelectedHours();
  QList<QDate> dRange = this->getSelectedDays();
  DateTimeList ranges;
  if (dRange.count()>0 && hRange.count()==1 && 
      hRange.at(0).first==QTime(0,0,0) &&
      hRange.at(0).second==QTime(23,59,59)) {
    dRange.append(dRange.back().addDays(2));
    int startDate = 0;
    int prev = dRange.at(0).toJulianDay();
    for (int i=0; i<dRange.count()-1; i++) {
      if (dRange.at(i).daysTo(dRange.at(i+1))>1) {
        ranges.append(qMakePair(QDateTime(dRange.at(startDate), hRange.at(0).first),
                                QDateTime(dRange.at(i), hRange.at(0).second)));
        startDate = i+1;
      }
    }
  }
  else {
    for (int i=0; i<dRange.count(); i++)
      for (int j=0; j<hRange.count(); j++)
        ranges.append(qMakePair(QDateTime(dRange.at(i), hRange.at(j).first),
                                QDateTime(dRange.at(i), hRange.at(j).second)));
  }
  return ranges;
}
