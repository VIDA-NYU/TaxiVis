#include "yellowtobrownscale.h"

#include <cmath>

QColor interpolate(const QColor &a, const QColor &b, float alpha) {
  int blue  = std::min((1.f - alpha) * a.blue()  + alpha * b.blue(), 255.f);
  int green = std::min((1.f - alpha) * a.green() + alpha * b.green(), 255.f);
  int red   = std::min((1.f - alpha) * a.red()   + alpha * b.red(), 255.f);
  return QColor(red, green, blue);
}

YellowToBrownScale::YellowToBrownScale() {
  _colors.resize(256);

  QColor yellow = Qt::yellow;
  QColor brown(139 ,69, 19);

  for (unsigned i=0; i<256; ++i) {
    _colors[i] = interpolate(yellow, brown, i/256.f);
  }
}

