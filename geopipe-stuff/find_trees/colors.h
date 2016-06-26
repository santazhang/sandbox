#pragma once

#include <inttypes.h>

namespace find_trees {

typedef struct {
    double r;       // percent
    double g;       // percent
    double b;       // percent
} rgb;

typedef struct {
    double h;       // angle in degrees
    double s;       // percent
    double v;       // percent
} hsv;

hsv rgb2hsv(rgb in);
rgb hsv2rgb(hsv in);


void fill_lab_a_channel(int n_pixels, uint8_t* lab_a, const uint8_t* red, const uint8_t* green, const uint8_t* blue);

}  // namespace find_trees
