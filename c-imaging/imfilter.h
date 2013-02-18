#pragma once

#include "im.h"

image imfilter_ident(image in, image out);

image imfilter_inv(image in, image out);

image imfilter_gray(image in, image out, double wt_r, double wt_g, double wt_b);

#define imfilter_intensity(in, out) imfilter_gray(in, out, 7471.0 / 65536.0, 38470.0 / 65536.0, 19595.0 / 65536.0);

image imfilter_contrast(image in, image out, double contrast);

image imfilter_tile(image in, image out, int size);

enum {
    IMFILTER_SHIAU_FAN,
    IMFILTER_FLOYD_STEINBERG,
    IMFILTER_OSTROMOUKHOV,
};
image imfilter_halftone(image in, image out, int method);

image imfilter_box_blur(image in, image out, int radius);

image imfilter_gaussian_blur(image in, image out, int radius);

image imfilter_conv(image in, image out, int n, int* dx_arr, int* dy_arr, double* w, uint8_t b);

image imfilter_pencil(image in, image out, double strength);

image imfilter_oil_paint(image in, image out, int radius, int details);

image imfilter_dilate(image in, image out, int radius);

image imfilter_erode(image in, image out, int radius);

image imfilter_opening(image in, image out, int radius);

image imfilter_closing(image in, image out, int radius);

image imfilter_watercolor_dark(image in, image out, int radius);

image imfilter_watercolor_bright(image in, image out, int radius);
