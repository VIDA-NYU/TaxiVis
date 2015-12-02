#include "colorbar.h"
#include "util/colorscale.h"

#include <QtGui>
#include <iostream>

ColorBar::ColorBar(ColorScale *colorScale):
    _colorScale(colorScale),
    _minReal(0.0),
    _maxReal(0.0),
    _minSel(0),
    _maxSel(0),
    _leftButtonPress(false) {
    _gradient = QLinearGradient(BAR_START_X, BAR_START_Y+BAR_HEIGHT,
                                BAR_START_X, BAR_START_Y);

    setColorScale(colorScale);
}

bool ColorBar::contains(const QPoint &p) {
    if (p.x()>=BAR_START_X && p.x() <= BAR_START_X+BAR_WIDTH &&
            p.y()>=BAR_START_Y-2 && p.y() <= BAR_START_Y+BAR_HEIGHT+2)
        return true;
    else {
        _leftButtonPress = false;
        return false;
    }

}

void ColorBar::setColorScale(ColorScale *colorScale) {
    _colorScale = colorScale;
    if (colorScale==NULL) return;

    // Set colors on gradient
    for (int i=0; i<BAR_HEIGHT; ++i) {
      _gradient.setColorAt(double(i)/(BAR_HEIGHT-1), _colorScale->getColor(float(i)/(BAR_HEIGHT-1)));
    }
}

void ColorBar::setRealMinMax(double min, double max) {
    _minReal = min;
    _maxReal = max;
}

void ColorBar::paint(QPainter *painter) {
    if (_colorScale==NULL) return;

    // Draw rectangle with gradient on widget
    painter->setPen(Qt::black);
    painter->setBrush(_gradient);
    painter->drawRect(BAR_START_X, BAR_START_Y, BAR_WIDTH, BAR_HEIGHT);

    // draw ticks and text
    if (_minReal!=_maxReal) {
        int    stepTick = BAR_HEIGHT/TICKS_NUM;
        //    for (int i=BAR_START_Y; i<=BAR_START_Y+BAR_HEIGHT; i+=stepTick) {
        for (int i=BAR_START_Y+BAR_HEIGHT; i>=BAR_START_Y; i-=stepTick) {
            painter->setPen( Qt::black);
            painter->drawLine(BAR_START_X+BAR_WIDTH, i,
                              BAR_START_X+BAR_WIDTH+TICK_WIDTH, i);

            double val = getRealValue(i);
            painter->setPen( Qt::white);
            painter->setBrush( Qt::white );
            painter->drawText(BAR_START_X+BAR_WIDTH+TICK_WIDTH+3, i-TEXT_HEIGHT/4, TEXT_WIDTH, TEXT_HEIGHT,
                              Qt::KeepAspectRatio,QString::number(val,'f', 2));

            painter->setPen(Qt::blue);
            painter->setBrush( Qt::black );
            painter->drawText(BAR_START_X+BAR_WIDTH+TICK_WIDTH+4, i-TEXT_HEIGHT/4-1, TEXT_WIDTH, TEXT_HEIGHT,
                              Qt::KeepAspectRatio,QString::number(val,'f', 2));
        }
 
        // draw unit
        if (!this->_unit.isEmpty()) {
          QFontMetrics metrics(painter->font());
          int width = metrics.width(this->_unit);
          painter->save();
          painter->translate(BAR_START_X-metrics.descent()-1, BAR_START_Y+BAR_HEIGHT/2);
          painter->rotate(270);
          painter->setPen(Qt::white);
          painter->drawText(-width/2, 0, this->_unit);
          painter->setPen(Qt::black);
          painter->drawText(-width/2+1, -1, this->_unit);
          painter->restore();
        }
    }

    // draw selection
    if (_minSel!=_maxSel) {
        QRect rect(BAR_START_X-2, _minSel, BAR_WIDTH+5, _maxSel-_minSel);
        painter->setPen(QPen(Qt::black, 2));
        painter->setBrush(QColor(128, 128, 128, 50));
        painter->drawRect(rect);
    }
}

bool ColorBar::processMouseEvent(QMouseEvent *event) {
    if (event->button()==Qt::LeftButton && event->type()==QEvent::MouseButtonPress){
        _leftButtonPress = true;
        _minSel = _maxSel = event->pos().y();
        return true;
    } else if (event->type()==QEvent::MouseMove && _leftButtonPress) {
        _maxSel = event->pos().y();

        if (Qt::ControlModifier != QApplication::keyboardModifiers()) {
            emit rangeSelectionChanged(getRealValue(std::max(_minSel, _maxSel)),
                                       getRealValue(std::min(_minSel, _maxSel)));
        }

        return true;
    } else if (event->button()==Qt::LeftButton && event->type()==QEvent::MouseButtonRelease) {
        _leftButtonPress = false;

        // Reset limits if Control button is pressed
        if (Qt::ControlModifier== QApplication::keyboardModifiers()) {
            emit rangeLimitsChanged(getRealValue(std::max(_minSel, _maxSel)),
                                      getRealValue(std::min(_minSel, _maxSel)));
            _minSel = _maxSel = 0;
        } else {
            emit rangeSelectionChanged(getRealValue(std::max(_minSel, _maxSel)),
                                       getRealValue(std::min(_minSel, _maxSel)));
        }

        return true;
    }

    // unselect
    if (event->button()==Qt::RightButton) {
        // clean selection
        _minSel = _maxSel = 0;
        if (Qt::ControlModifier== QApplication::keyboardModifiers()) {
            emit rangelimitsReseted();
        } else {
            emit rangeSelectionChanged(getRealValue(std::max(_minSel, _maxSel)),
                                       getRealValue(std::min(_minSel, _maxSel)));
        }
        return true;
    }
    return false;
}

double ColorBar::getRealValue(int value) {
    return _minReal+(_maxReal-_minReal)*(BAR_START_Y+BAR_HEIGHT-value+1e-10)/BAR_HEIGHT;
}

double ColorBar::getRealMin()
{
  return this->_minReal;
}

double ColorBar::getRealMax()
{
  return this->_maxReal;
}

void ColorBar::setUnit(const QString &unit)
{
  this->_unit = unit;
}
