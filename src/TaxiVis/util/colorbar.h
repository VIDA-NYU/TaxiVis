#ifndef COLORBAR_H
#define COLORBAR_H

#include <QLinearGradient>

class ColorScale;
class QMouseEvent;

class ColorBar: public QObject {
  Q_OBJECT

  // ColorBar parameters.
  enum baseSize {
    BAR_START_X = 30,
    BAR_START_Y = 60,
    BAR_WIDTH   = 20,
    BAR_HEIGHT  = 256,
    TICK_WIDTH  = 5,
    TICKS_NUM   = 4,
    TEXT_START_X= 20,
    TEXT_START_Y= 80,
    TEXT_WIDTH  = 200,
    TEXT_HEIGHT = 30
  };

public:
  explicit ColorBar(ColorScale *colorScale=NULL);
  bool contains(const QPoint &p);
  double getRealMin();
  double getRealMax();
  void setUnit(const QString &unit);

signals:
  bool rangeSelectionChanged(double min, double max);
  bool rangeLimitsChanged(double min, double max);
  bool rangelimitsReseted();

public slots:
  void setColorScale(ColorScale *colorScale);
  void setRealMinMax(double min, double max);
  void paint(QPainter *painter);
  bool processMouseEvent(QMouseEvent *event);

protected:
  double getRealValue(int value);

private:
  ColorScale     *_colorScale;
  QLinearGradient _gradient;
  double          _minReal;
  double          _maxReal;
  int             _minSel;
  int             _maxSel;
  bool            _leftButtonPress;
  QString         _unit;
};

#endif // COLORBAR_H
