#include "colorscale.h"
#include "util/pseudorainbowscale.h"
#include "util/bluetocyanscale.h"
#include "util/bluetoyellowscale.h"
#include "util/heatedobjectscale.h"
#include "util/grayscale.h"
#include "util/lineargrayscale.h"
#include "util/locsscale.h"
#include "util/rainbowscale.h"
#include "util/greentowhitescale.h"
#include "util/yellowtobrownscale.h"
#include "util/divergent.h"
#include "util/sequentialred.h"

#include <iostream>
#include <cmath>
#include <cassert>

ColorScale* ColorScaleFactory::getInstance(ColorScaleType type) {
    if (type == PSEUDO_RAINBOW_SCALE) {
        return new PseudoRainbowScale();
    } else if (type == BLUE_TO_CYAN) {
        return new BlueToCyanScale();
    } else if (type == BLUE_TO_YELLOW_SCALE) {
        return new BlueToYellowScale();
    } else if (type == HEATED_OBJECTS) {
        return new HeatedObjectScale();
    } else if (type == GRAY_SCALE) {
        return new GrayScale();
    } else if (type == LINEAR_GRAY_SCALE) {
        return new LinearGrayScale();
    } else if (type == LOCS_SCALE) {
        return new LocsScale();
    } else if (type == RAINBOW_SCALE) {
        return new RainbowScale();
    } else if (type == GREEN_TO_WHITE_SCALE) {
        return new GreenToWhiteScale();
    } else if (type == YELLOW_TO_BROWN_SCALE) {
        return new YellowToBrownScale();
    } else if (type == DIVERGENT) {
        return new Divergent();
    } else if(type == SEQUENTIAL_SINGLE_HUE_RED){
        return new SequentialRed();
    }
    return NULL;
}


ColorScale::ColorScale():
    _min(0.f),
    _max(1.f),
    _reverse(false){
}

QColor ColorScale::getColor(float value) {
    if (_reverse) {
        int minindex = (int) ((_colors.size() - 1) * (1 - _min));
        int maxindex = (int) ((_colors.size() - 1) * (1 - _max));
        //std::cout << "index " << index << std::endl;
        int index = (int) (std::abs(minindex - maxindex) * (1 - value)) + maxindex;
        return _colors[index];
    } else {
        int minindex = (int) ((_colors.size() - 1) * _min);
        int maxindex = (int) ((_colors.size() - 1) * _max);
        int index = (int) (round(std::abs(maxindex - minindex) * value)) + minindex;
        //std::cout << "index " << index << std::endl;
        return _colors[index];
    }
}

void ColorScale::setMinMax(float min, float max) {
    assert(max >= min);
    _max = max;
    _min = min;
}

