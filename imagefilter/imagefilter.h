#pragma once

#include <stdint.h>
#include <string.h>

#ifndef __APPLE__
#include <malloc.h>
#else
#include <stdlib.h>
#endif // __APPLE__

typedef uint8_t u8;

typedef struct {
    int width;
    int height;
    u8** data;
} _image, *image;

typedef u8* pixel;

#define px_r(px) (*px)
#define px_g(px) (*(px + 1))
#define px_b(px) (*(px + 2))

#define im_pixel(im, x, y) (im->data[(y)] + (x) * 3)

image im_new(int width, int height);

void im_free(image im);

image im_inv(image in, image out);

image im_gray(image in, image out, double r_weight, double g_weight, double b_weight);

image im_tile(image in, image out, int tile_size);

image im_contrast(image in, image out, double slope);

image im_avg_blur(image in, image out, double radius);

image im_dilate(image in, image out, double radius);

image im_erode(image in, image out, double radius);

image im_opening(image in, image out, double radius);

image im_closing(image in, image out, double radius);

image im_watercolor_dark(image in, image out, double radius);

image im_watercolor_bright(image in, image out, double radius);

image im_matrix_op(image in, image out, int n, int* dxy, double* w, double b, u8 def_r, u8 def_g, u8 def_b);

image im_pencil_sketch(image in, image out, double strength, double brightness);

image im_edge_detect(image in, image out, int dx, int dy, u8 bias);

image im_edge_enhance(image in, image out, double strength);
