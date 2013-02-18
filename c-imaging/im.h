#pragma once

#include <stdint.h>

#ifndef __APPLE__
#include <malloc.h>
#else
#include <stdlib.h>
#endif // __APPLE__

typedef uint8_t* pixel;

#define px_r(px) (*(px))
#define px_g(px) (*(px + 1))
#define px_b(px) (*(px + 2))

typedef struct __image {
    int width;
    int height;
    uint8_t** data;
} *image;

#define im_pixel(im, x, y) (im->data[(y)] + (x) * 3)

#define im_pixel_clamp(im, x, y) im_pixel(im, clamp((x), 0, im->width - 1), clamp((y), 0, im->height - 1))

#define clamp(val, min_val, max_val) ((val) < (min_val) ? (min_val) : ((val) > (max_val) ? (max_val) : (val)))

#define clamp_rgb(val) clamp((val), 0, 255)

image im_new(int width, int height);

image im_dup(image im);

void im_free(image im);
