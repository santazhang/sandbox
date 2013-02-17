#include "imagefilter.h"


#define _limit2(c, a, b) ((c) < (a) ? (a) : ((c) > (b) ? (b) : (c)))
#define _limit(c) _limit2(c, 0, 255)

image im_new(int width, int height) {
    image im = (image) malloc(sizeof(_image));
    im->width = width;
    im->height = height;
    im->data = (u8 **) malloc(sizeof(u8*) * im->height);
    int i, row_bytes = sizeof(u8) * im->width * 3;
    for (i = 0; i < im->height; i++) {
        im->data[i] = (u8 *) malloc(row_bytes);
        memset(im->data[i], 0, row_bytes);
    }
    return im;
}

void im_free(image im) {
    int i;
    for (i = 0; i < im->height; i++) {
        free(im->data[i]);
    }
    free(im->data);
}

image im_inverse(image in, image out) {
    int x, y;
    for (y = 0; y < in->height; y++) {
        for (x= 0; x < in->width; x++) {
            pixel px = im_pixel(in, x, y);
            pixel px_out = im_pixel(out, x, y);
            px_r(px_out) = 255 - px_r(px);
            px_g(px_out) = 255 - px_g(px);
            px_b(px_out) = 255 - px_b(px);
        }
    }
}

image im_gray(image in, image out, double r_weight, double g_weight, double b_weight) {
    int x, y;
    for (y = 0; y < in->height; y++) {
        for (x= 0; x < in->width; x++) {
            pixel px = im_pixel(in, x, y);
            double v = px_r(px) * r_weight + px_g(px) * g_weight + px_b(px) * b_weight;
            u8 g = (u8) _limit(v);
            px = im_pixel(out, x, y);
            px_r(px) = g;
            px_g(px) = g;
            px_b(px) = g;
        }
    }
}

image im_intensity(image in, image out) {
    return im_gray(in, out, 7471.0 / 65536.0, 38469.0 / 65536.0, 19595.0 / 65536.0);
}

image im_contrast(image in, image out, double slope) {
    int x, y;
    for (y = 0; y < in->height; y++) {
        for (x= 0; x < in->width; x++) {
            pixel px = im_pixel(in, x, y);
            pixel px_out = im_pixel(out, x, y);
            double vr = (px_r(px) - 127) * slope + 127;
            double vg = (px_g(px) - 127) * slope + 127;
            double vb = (px_b(px) - 127) * slope + 127;
            px_r(px_out) = (u8) _limit(vr);
            px_g(px_out) = (u8) _limit(vg);
            px_b(px_out) = (u8) _limit(vb);
        }
    }
    return out;
}

image im_tile(image in, image out, int tile_size) {
    int tile_x, tile_y, x, y;
    for (tile_y = 0; tile_y * tile_size < in->height; tile_y++) {
        for (tile_x = 0; tile_x * tile_size < in->width; tile_x++) {
            double sr = 0, sg = 0, sb = 0;
            int n = 0;
            for (y = tile_y * tile_size; y < (tile_y + 1) * tile_size && y < in->height; y++) {
                for (x = tile_x * tile_size; x < (tile_x + 1) * tile_size && x < in->width; x++) {
                    n++;
                    pixel px = im_pixel(in, x, y);
                    sr += px_r(px);
                    sg += px_g(px);
                    sb += px_b(px);
                }
            }
            u8 r = (u8) _limit(sr / n);
            u8 g = (u8) _limit(sg / n);
            u8 b = (u8) _limit(sb / n);
            for (y = tile_y * tile_size; y < (tile_y + 1) * tile_size && y < in->height; y++) {
                for (x = tile_x * tile_size; x < (tile_x + 1) * tile_size && x < in->width; x++) {
                    pixel px = im_pixel(out, x, y);
                    px_r(px) = r;
                    px_g(px) = g;
                    px_b(px) = b;
                }
            }
        }
    }
    return out;
}


image im_avg_blur(image in, image out, double radius) {
    int inplace = (in == out);
    if (out == NULL || inplace) {
        out = im_new(in->width, in->height);
    }

    // TODO diff based O(n^2) algorithm
    // hint: as in im_oil_color, use the north, south, west, and east edge of the brush (this could be precalculated)
    // the sum in the brush area could be precalculated
    int x, y, dx, dy;
    double radius2 = radius * radius;
    for (y = 0; y < in->height; y++) {
        for (x = 0; x < in->width; x++) {
            int n = 0;
            double sr = 0, sg = 0, sb = 0;
            for (dx = -radius; dx <= radius; dx++) {
                for (dy = -radius; dy <= radius; dy++) {
                    if (dx * dx + dy * dy > radius2 || x + dx < 0 || x + dx >= in->width || y + dy < 0 || y + dy >= in->height) {
                        continue;
                    }
                    n++;
                    pixel px = im_pixel(in, x + dx, y + dy);
                    u8 r = px_r(px), g = px_g(px), b = px_b(px);
                    sr += r;
                    sg += g;
                    sb += b;
                }
            }
            pixel px = im_pixel(out, x, y);
            px_r(px) = (u8) _limit(sr / n);
            px_g(px) = (u8) _limit(sg / n);
            px_b(px) = (u8) _limit(sb / n);
        }
    }

    if (inplace) {
        int y;
        for (y = 0; y < in->height; y++) {
            memcpy(in->data[y], out->data[y], in->width * 3);
        }
        im_free(out);
        out = in;
    }
    return out;
}

image im_dilate(image in, image out, double radius) {
    int inplace = (in == out);
    if (out == NULL || inplace) {
        out = im_new(in->width, in->height);
    }

    // TODO diff based O(n^2) algorithm
    // hint: as in im_oil_color, use the north, south, west, and east edge of the brush (this could be precalculated)
    // the value in the brush area could be precaculated, and efficiently stored in a Segment Tree (log128, const time update for each op)
    int x, y, dx, dy;
    double radius2 = radius * radius;
    for (y = 0; y < in->height; y++) {
        for (x = 0; x < in->width; x++) {
            u8 r = 0, g = 0, b = 0;
            for (dx = -radius; dx <= radius; dx++) {
                for (dy = -radius; dy <= radius; dy++) {
                    if (dx * dx + dy * dy > radius2 || x + dx < 0 || x + dx >= in->width || y + dy < 0 || y + dy >= in->height) {
                        continue;
                    }
                    pixel px = im_pixel(in, x + dx, y + dy);
                    u8 r2 = px_r(px), g2 = px_g(px), b2 = px_b(px);
                    r = (r > r2) ? r : r2;
                    g = (g > g2) ? g : g2;
                    b = (b > b2) ? b : b2;
                }
            }
            pixel px = im_pixel(out, x, y);
            px_r(px) = r;
            px_g(px) = g;
            px_b(px) = b;
        }
    }

    if (inplace) {
        int y;
        for (y = 0; y < in->height; y++) {
            memcpy(in->data[y], out->data[y], in->width * 3);
        }
        im_free(out);
        out = in;
    }
    return out;
}

image im_erode(image in, image out, double radius) {
    int inplace = (in == out);
    if (out == NULL || inplace) {
        out = im_new(in->width, in->height);
    }

    // TODO diff based O(n^2) algorithm
    // hint: as in im_oil_color, use the north, south, west, and east edge of the brush (this could be precalculated)
    // the value in the brush area could be precaculated, and efficiently stored in a Segment Tree (log128, const time update for each op)
    int x, y, dx, dy;
    double radius2 = radius * radius;
    for (y = 0; y < in->height; y++) {
        for (x = 0; x < in->width; x++) {
            u8 r = 255, g = 255, b = 255;
            for (dx = -radius; dx <= radius; dx++) {
                for (dy = -radius; dy <= radius; dy++) {
                    if (dx * dx + dy * dy > radius * radius || x + dx < 0 || x + dx >= in->width || y + dy < 0 || y + dy >= in->height) {
                        continue;
                    }
                    pixel px = im_pixel(in, x + dx, y + dy);
                    u8 r2 = px_r(px), g2 = px_g(px), b2 = px_b(px);
                    r = (r < r2) ? r : r2;
                    g = (g < g2) ? g : g2;
                    b = (b < b2) ? b : b2;
                }
            }
            pixel px = im_pixel(out, x, y);
            px_r(px) = r;
            px_g(px) = g;
            px_b(px) = b;
        }
    }

    if (inplace) {
        int y;
        for (y = 0; y < in->height; y++) {
            memcpy(in->data[y], out->data[y], in->width * 3);
        }
        im_free(out);
        out = in;
    }
    return out;
}

image im_opening(image in, image out, double radius) {
    image tmp = im_new(in->width, in->height);
    im_erode(in, tmp, radius);
    im_dilate(tmp, out, radius);
    im_free(tmp);
    return out;
}

image im_closing(image in, image out, double radius) {
    image tmp = im_new(in->width, in->height);
    im_dilate(in, tmp, radius);
    im_erode(tmp, out, radius);
    im_free(tmp);
    return out;
}

image im_watercolor_dark(image in, image out, double radius) {
    // opening -> closing
    // TODO avoid creating too many images
    image tmp = im_new(in->width, in->height);
    im_opening(in, tmp, radius);
    im_closing(tmp, out, radius);
    im_free(tmp);
    return out;
}

image im_watercolor_bright(image in, image out, double radius) {
    // closing -> opening
    // TODO avoid creating too many images
    image tmp = im_new(in->width, in->height);
    im_closing(in, tmp, radius);
    im_opening(tmp, out, radius);
    im_free(tmp);
    return out;
}

image im_matrix_op(image in, image out, int n, int* dxy, double* w, double b, u8 def_r, u8 def_g, u8 def_b) {
    int inplace = (in == out);
    if (out == NULL || inplace) {
        out = im_new(in->width, in->height);
    }

    int x, y, dx, dy, i;
    for (y = 0; y < in->height; y++) {
        for (x = 0; x < in->width; x++) {
            pixel px_out = im_pixel(out, x, y);
            double vr = b, vg = b, vb = b;
            for (i = 0; i < n; i++) {
                int nx = x + dxy[i * 2];
                int ny = y + dxy[i * 2 + 1];
                if (nx < 0 || nx >= in->width || ny < 0 || ny >= in->height) {
                    vr += w[i] * def_r;
                    vg += w[i] * def_g;
                    vb += w[i] * def_b;
                } else {
                    pixel px = im_pixel(in, nx, ny);
                    vr += w[i] * px_r(px);
                    vg += w[i] * px_g(px);
                    vb += w[i] * px_b(px);
                }
            }
            px_r(px_out) = (u8) _limit(vr);
            px_g(px_out) = (u8) _limit(vg);
            px_b(px_out) = (u8) _limit(vb);
        }
    }

    if (inplace) {
        int y;
        for (y = 0; y < in->height; y++) {
            memcpy(in->data[y], out->data[y], in->width * 3);
        }
        im_free(out);
        out = in;
    }
    return out;
}

image im_pencil_sketch(image in, image out, double strength, double brightness) {
    int n = 5;
    int dxy[] = {0, 0,
                 -1, 0,
                 1, 0,
                 0, -1,
                 0, 1};
    double w[] = {4 * strength,
                  -1 * strength,
                  -1 * strength,
                  -1 * strength,
                  -1 * strength};
    double b = brightness;
    u8 def_r = 255, def_g = 255, def_b = 255;
    return im_matrix_op(in, out, n, dxy, w, b, def_r, def_g, def_b);
}

image im_edge_detect(image in, image out, int dx, int dy, u8 bias) {
    int n = 2;
    int dxy[] = {0, 0,
                 dx, dy};
    double w[] = {1,
                  -1};
    double b = bias;
    u8 def_r = 127, def_g = 127, def_b = 127;
    image tmp = im_new(in->width, in->height);
    im_gray(in, tmp, 1/3.0, 1/3.0, 1/3.0);
    im_matrix_op(tmp, tmp, n, dxy, w, b, def_r, def_g, def_b);
    im_inverse(tmp, out);
    im_free(tmp);
    return out;
}

image im_edge_enhance(image in, image out, double strength) {
    int n = 5;
    int dxy[] = {0, 0,
                 -1, 0,
                 1, 0,
                 0, -1,
                 0, 1};
    double w[] = {5 * strength,
                  -1 * strength,
                  -1 * strength,
                  -1 * strength,
                  -1 * strength};
    double b = 0;
    u8 def_r = 255, def_g = 255, def_b = 255;
    return im_matrix_op(in, out, n, dxy, w, b, def_r, def_g, def_b);
}

pixel _im_pixel_safe(image im, int x, int y) {
    x = _limit2(x, 0, im->width - 1);
    y = _limit2(y, 0, im->height - 1);
    return im_pixel(im, x, y);
}

#define _intensity(px) ((px_r(px) * 7471 + px_g(px) * 38469 + px_b(px) * 19595) / 65536)

int _level_scaling(int v, int levels) {
    int r = v * levels + 128;
    r = ((r >> 8) + r) >> 8;
    return r;
}

image im_oil_paint(image in, image out, int radius, int details) {
    int inplace = (in == out);
    if (out == NULL || inplace) {
        out = im_new(in->width, in->height);
    }

    int levels = 1 + details;
    int* intensity_count = (int *) malloc(sizeof(int) * levels);
    memset(intensity_count, 0, sizeof(int) * levels);
    long* sum_red = (long *) malloc(sizeof(long) * levels);
    memset(sum_red, 0, sizeof(long) * levels);
    long* sum_green = (long *) malloc(sizeof(long) * levels);
    memset(sum_green, 0, sizeof(long) * levels);
    long* sum_blue = (long *) malloc(sizeof(long) * levels);
    memset(sum_blue, 0, sizeof(long) * levels);

    int x, y;

    // init
    for (y = -radius; y <= radius; y++) {
        for (x = -radius; x <= radius; x++) {
            pixel px = _im_pixel_safe(in, x, y);
            int intensity = _intensity(px);
            int intensity_level = _level_scaling(intensity, details);
            intensity_count[intensity_level]++;
            sum_red[intensity_level] += px_r(px);
            sum_green[intensity_level] += px_g(px);
            sum_blue[intensity_level] += px_b(px);
        }
    }

    y = 0;
    for (x = 0; x < in->width; x++) {
        int direction = (x & 0x1) ? -1 : 1;
        int counter = 0;
        int i, d;
        for (;;) {
            int max_idx = 0;
            for (i = 0; i < levels; i++) {
                if (intensity_count[i] > intensity_count[max_idx]) {
                    max_idx = i;
                }
            }

            double dr = 1.0 * sum_red[max_idx] / intensity_count[max_idx];
            u8 r = (u8) _limit(dr);
            double dg = 1.0 * sum_green[max_idx] / intensity_count[max_idx];
            u8 g = (u8) _limit(dg);
            double db = 1.0 * sum_blue[max_idx] / intensity_count[max_idx];
            u8 b = (u8) _limit(db);

            pixel px = im_pixel(out, x, y);
            px_r(px) = r;
            px_g(px) = g;
            px_b(px) = b;

            counter++;
            if (counter != in->height) {
                for (d = -radius; d <= radius; d++) {
                    pixel px = _im_pixel_safe(in, x + d, y - direction * radius);
                    int intensity = _intensity(px);
                    int intensity_level = _level_scaling(intensity, details);
                    intensity_count[intensity_level]--;
                    sum_red[intensity_level] -= px_r(px);
                    sum_green[intensity_level] -= px_g(px);
                    sum_blue[intensity_level] -= px_b(px);
                }
                y += direction;
                for (d = -radius; d <= radius; d++) {
                    pixel px = _im_pixel_safe(in, x + d, y + direction * radius);
                    int intensity = _intensity(px);
                    int intensity_level = _level_scaling(intensity, details);
                    intensity_count[intensity_level]++;
                    sum_red[intensity_level] += px_r(px);
                    sum_green[intensity_level] += px_g(px);
                    sum_blue[intensity_level] += px_b(px);
                }
            } else {
                for (d = -radius; d <= radius; d++) {
                    pixel px = _im_pixel_safe(in, x - radius, y + d);
                    int intensity = _intensity(px);
                    int intensity_level = _level_scaling(intensity, details);
                    intensity_count[intensity_level]--;
                    sum_red[intensity_level] -= px_r(px);
                    sum_green[intensity_level] -= px_g(px);
                    sum_blue[intensity_level] -= px_b(px);
                }
                for (d = -radius; d <= radius; d++) {
                    pixel px = _im_pixel_safe(in, x + radius + 1, y + d);
                    int intensity = _intensity(px);
                    int intensity_level = _level_scaling(intensity, details);
                    intensity_count[intensity_level]++;
                    sum_red[intensity_level] += px_r(px);
                    sum_green[intensity_level] += px_g(px);
                    sum_blue[intensity_level] += px_b(px);
                }
                break;
            }
        }
    }

    free(intensity_count);
    free(sum_red);
    free(sum_green);
    free(sum_blue);

    if (inplace) {
        int y;
        for (y = 0; y < in->height; y++) {
            memcpy(in->data[y], out->data[y], in->width * 3);
        }
        im_free(out);
        out = in;
    }
    return out;
}

image im_halftone_error_diffusion(image in, image out, int n, int* xoff, int* yoff, double* w) {
    int x, y, i;
    int** error = (int **) malloc(sizeof(int *) * in->height);
    for (y = 0; y < in->height; y++) {
        error[y] = (int *) malloc(sizeof(int) * in->width);
        memset(error[y], 0, sizeof(int) * in->width);
    }

    for (y = 0; y < in->height; y++) {
        for (x = 0; x < in->width; x++) {
            pixel px = im_pixel(in, x, y);
            int g = (px_r(px) + px_g(px) + px_b(px)) / 3;

            // distance from dark
            int ddark = (g - 0) + error[y][x];

            // distance fron bright
            int dbright = (g - 255) + error[y][x];

            // error
            int e = 0;

            if (abs(ddark) < abs(dbright)) {
                pixel px = im_pixel(out, x, y);
                px_r(px) = 0;
                px_g(px) = 0;
                px_b(px) = 0;
                e = ddark;
            } else {
                pixel px = im_pixel(out, x, y);
                px_r(px) = 255;
                px_g(px) = 255;
                px_b(px) = 255;
                e = dbright;
            }

            for (i = 0; i < n; i++) {
                int nx = x + xoff[i];
                int ny = y + yoff[i];
                if (0 <= nx && nx < in->width && 0 <= ny && ny < in->height) {
                    error[ny][nx] += e * w[i];
                }
            }
        }
    }

    for (y = 0; y < in->height; y++) {
        free(error[y]);
    }
    free(error);
    return out;
}

image im_halftone_shiau_fan(image in, image out) {
    int n = 5;
    int xoff[] = {-3, -2, -1, 0, 1};
    int yoff[] = {1, 1, 1, 1, 0};
    double w[] = {0.0625, 0.0625, 0.125, 0.25, 0.5};
    im_halftone_error_diffusion(in, out, n, xoff, yoff, w);
}

image im_halftone_floyd_steinberg(image in, image out) {
    int n = 4;
    int xoff[] = {-1, 0, 1, 1};
    int yoff[] = {1, 1, 1, 0};
    double w[] = {0.1875, 0.3125, 0.0625, 0.4375};
    im_halftone_error_diffusion(in, out, n, xoff, yoff, w);
}

image im_halftone_ostromoukhov(image in, image out) {
    int n = 3;
    int xoff[] = {1, -1, 0};
    int yoff[] = {0, 1, 1};

    double ostromoukhov_w[256][3];

#define ostromoukhov_weight(idx, v0, v1, v2) {ostromoukhov_w[idx][0] = v0, ostromoukhov_w[idx][1] = v1, ostromoukhov_w[idx][2] = v2;}

    ostromoukhov_weight(0, 13.0 / 18.0, 0, 5.0 / 18.0);
    ostromoukhov_weight(1, 13.0 / 18.0, 0, 5.0 / 18.0);
    ostromoukhov_weight(2, 21.0 / 31.0, 0, 10.0 / 31.0);
    ostromoukhov_weight(3, 7.0 / 11.0, 0, 4.0 / 11.0);
    ostromoukhov_weight(4, 8.0 / 13.0, 0, 5.0 / 13.0);
    ostromoukhov_weight(10, 7.0 / 13.0, 3.0 / 13.0, 3.0 / 13.0);

#define ostromoukhov_interpolate(start, stop) { \
        double step[3] = { \
            (ostromoukhov_w[stop][0] - ostromoukhov_w[start][0]) / (stop - start), \
            (ostromoukhov_w[stop][1] - ostromoukhov_w[start][1]) / (stop - start), \
            (ostromoukhov_w[stop][2] - ostromoukhov_w[start][2]) / (stop - start) \
        }; \
        int i; \
        for (i = start + 1; i < stop; i++) { \
            ostromoukhov_w[i][0] = ostromoukhov_w[start][0] + (i - start) * step[0]; \
            ostromoukhov_w[i][1] = ostromoukhov_w[start][1] + (i - start) * step[1]; \
            ostromoukhov_w[i][2] = ostromoukhov_w[start][2] + (i - start) * step[2]; \
        } \
    }

    ostromoukhov_interpolate(4, 10);

    ostromoukhov_weight(22, 0.5, 1.0 / 3.0, 1.0 / 6.0);

    ostromoukhov_interpolate(10, 22);

    ostromoukhov_weight(32, 20.0 / 49.0, 10.0 / 49.0, 19.0 / 49.0);

    ostromoukhov_interpolate(22, 32);

    ostromoukhov_weight(64, 11.0 / 21.0, 10.0 / 21.0, 0);

    ostromoukhov_interpolate(32, 64);

    ostromoukhov_weight(72, 5.0 / 13.0, 5.0 / 13.0, 1.0 / 13.0);

    ostromoukhov_interpolate(64, 72);

    ostromoukhov_weight(77, 2.0 / 3.0, 1.0 / 6.0, 1.0 / 6.0);

    ostromoukhov_interpolate(72, 77);

    ostromoukhov_weight(85, 2.0 / 3.0, 1.0 / 6.0, 1.0 / 6.0);

    ostromoukhov_interpolate(77, 85);

    ostromoukhov_weight(95, 0.5, 0.3, 0.2);

    ostromoukhov_interpolate(85, 95);

    ostromoukhov_weight(107, 0.5, 0.3, 0.2);

    ostromoukhov_interpolate(95, 107);

    ostromoukhov_weight(127, 2.0 / 3.0, 1.0 / 6.0, 1.0 / 6.0);

    ostromoukhov_interpolate(107, 127);

    int i;
    for (i = 0; i < 128; i++) {
        ostromoukhov_w[255 - i][0] = ostromoukhov_w[i][0];
        ostromoukhov_w[255 - i][1] = ostromoukhov_w[i][1];
        ostromoukhov_w[255 - i][2] = ostromoukhov_w[i][2];
    }

#undef ostromoukhov_weight
#undef ostromoukhov_interpolate

    int x, y;
    int** error = (int **) malloc(sizeof(int *) * in->height);
    for (y = 0; y < in->height; y++) {
        error[y] = (int *) malloc(sizeof(int) * in->width);
        memset(error[y], 0, sizeof(int) * in->width);
    }

    for (y = 0; y < in->height; y++) {
        for (x = 0; x < in->width; x++) {
            pixel px = im_pixel(in, x, y);
            int g = (px_r(px) + px_g(px) + px_b(px)) / 3;

            // distance from dark
            int ddark = (g - 0) + error[y][x];

            // distance fron bright
            int dbright = (g - 255) + error[y][x];

            // error
            int e = 0;

            if (abs(ddark) < abs(dbright)) {
                pixel px = im_pixel(out, x, y);
                px_r(px) = 0;
                px_g(px) = 0;
                px_b(px) = 0;
                e = ddark;
            } else {
                pixel px = im_pixel(out, x, y);
                px_r(px) = 255;
                px_g(px) = 255;
                px_b(px) = 255;
                e = dbright;
            }

            for (i = 0; i < n; i++) {
                int nx = x + xoff[i];
                int ny = y + yoff[i];
                if (0 <= nx && nx < in->width && 0 <= ny && ny < in->height) {
                    error[ny][nx] += e * ostromoukhov_w[g][i];
                }
            }
        }
    }

    for (y = 0; y < in->height; y++) {
        free(error[y]);
    }
    free(error);
    return out;
}
