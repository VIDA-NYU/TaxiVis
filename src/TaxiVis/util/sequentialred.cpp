#include "sequentialred.h"

SequentialRed::SequentialRed()
{
    _colors.resize(9);
    _colors[  0] = QColor(255, 245, 240);
    _colors[  1] = QColor(254, 224, 210);
    _colors[  2] = QColor(252, 187, 161);
    _colors[  3] = QColor(252, 146, 114);
    _colors[  4] = QColor(251, 106,  74);
    _colors[  5] = QColor(239,  59,  44);
    _colors[  6] = QColor(203,  24,  29);
    _colors[  7] = QColor(165,  15,  21);
    _colors[  8] = QColor(103,   0,  13);
}
