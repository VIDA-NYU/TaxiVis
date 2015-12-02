#ifndef COLORSCALE_H
#define COLORSCALE_H

#include <QtGui>
#include <vector>

using std::vector;

enum ColorScaleType {
    PSEUDO_RAINBOW_SCALE,
    BLUE_TO_CYAN,
    BLUE_TO_YELLOW_SCALE,
    HEATED_OBJECTS,
    GRAY_SCALE,
    LINEAR_GRAY_SCALE,
    LOCS_SCALE,
    RAINBOW_SCALE,
    GREEN_TO_WHITE_SCALE,
    YELLOW_TO_BROWN_SCALE,
    DIVERGENT,
    SEQUENTIAL_SINGLE_HUE_RED
};

class ColorScale;

class ColorScaleFactory {
public:
    static ColorScale* getInstance(ColorScaleType type);
};

class ColorScale {
public:
    ColorScale();

    // main function
    QColor getColor(float value);

    // getters
    float max()      { return _max; }
    float min()      { return _min; }
    bool isReverse() { return _reverse; }

    // setters
    void setMinMax(float min, float max);
    void setReverse(bool reverse) { _reverse = reverse; }

protected:
    float          _min;
    float          _max;
    bool           _reverse;
    vector<QColor> _colors;
    unsigned       _levels;
};

#endif // COLORSCALE_H
