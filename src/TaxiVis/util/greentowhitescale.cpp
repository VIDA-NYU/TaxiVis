#include "greentowhitescale.h"

GreenToWhiteScale::GreenToWhiteScale():ColorScale() {
  _colors.resize(256);

  int mid = (int) (_colors.size() * 0.65f);

  for (int i = 0; i < mid; i++) {
    int value = (int) (((float) i / (float) mid) * 255);
    _colors[i] = QColor(0, value, 0);
  }

  for (unsigned i = mid; i < _colors.size(); i++) {
    int value = (int) (((float) (i - mid) / (float) (_colors.size() - 1 - mid)) * 255);
    _colors[i] = QColor(value, 255, value);
  }
}
