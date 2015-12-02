#include "pseudorainbowscale.h"
#include <cmath>

PseudoRainbowScale::PseudoRainbowScale():ColorScale() {
  _colors.resize(256);

  float dx = 0.8f;

  for (unsigned i = 0; i < _colors.size(); i++) {
    float aux = (float) i / (_colors.size() - 1);
    float scaled = (6 - 2 * dx) * aux + dx;

    float r = std::max(0.f, (3 - std::abs(scaled - 4.f) - std::abs(scaled - 5.f)) / 2.f);
    float g = std::max(0.f, (4 - std::abs(scaled - 2.f) - std::abs(scaled - 4.f)) / 2.f);
    float b = std::max(0.f, (3 - std::abs(scaled - 1.f) - std::abs(scaled - 2.f)) / 2.f);

    _colors[i] = QColor(r*255, g*255, b*255);
  }
}
