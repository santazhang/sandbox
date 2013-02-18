#include <math.h>
#include <string.h>

#include "imfilter.h"

#define _min(x, y) ((x) < (y) ? (x) : (y))
#define _max(x, y) ((x) > (y) ? (x) : (y))
#define _px_intensity(px) ((px_r(px) * 7471 + px_g(px) * 38470 + px_b(px) * 19595) / 65536)

/*
    // point operation, can be directly applied in place
    image point_filter(image in, image out) {
        if (out == NULL) {
            out = im_new(in->width, in->height);
        }

        // do work

        return out;
    }

    // range operation, need temporary copy
    image range_filter(image in, image out) {
        int inplace = (in == out);
        if (out == NULL || inplace) {
            out = im_new(in->width, in->height);
        }

        // do work

        if (inplace) {
            imfilter_ident(out, in);
            im_free(out);
            out = in;
        }
        return out;
    }
*/


image imfilter_ident(image in, image out) {
    if (in == out) {
        return out;
    }
    if (out == NULL) {
        return im_dup(in);
    }
    int width = _min(in->width, out->width);
    int height = _min(in->height, out->height);
    int i, row_bytes = width * 3;
    for (i = 0; i < height; i++) {
        memcpy(out->data[i], in->data[i], row_bytes);
    }
    return out;
}


image imfilter_inv(image in, image out) {
    if (out == NULL) {
        out = im_new(in->width, in->height);
    }

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

    return out;
}


image imfilter_gray(image in, image out, double wt_r, double wt_g, double wt_b) {
    if (out == NULL) {
        out = im_new(in->width, in->height);
    }

    int x, y;
    for (y = 0; y < in->height; y++) {
        for (x= 0; x < in->width; x++) {
            pixel px = im_pixel(in, x, y);
            pixel px_out = im_pixel(out, x, y);
            double v = px_r(px) * wt_r + px_g(px) * wt_g + px_b(px) * wt_b;
            uint8_t g = (uint8_t) clamp_rgb(v);
            px_r(px_out) = g;
            px_g(px_out) = g;
            px_b(px_out) = g;
        }
    }

    return out;
}


image imfilter_contrast(image in, image out, double contrast) {
    if (out == NULL) {
        out = im_new(in->width, in->height);
    }

    int x, y;
    for (y = 0; y < in->height; y++) {
        for (x= 0; x < in->width; x++) {
            pixel px = im_pixel(in, x, y);
            pixel px_out = im_pixel(out, x, y);
            double nr = (px_r(px) - 127) * contrast + 127;
            double ng = (px_g(px) - 127) * contrast + 127;
            double nb = (px_b(px) - 127) * contrast + 127;
            px_r(px_out) = (uint8_t) clamp_rgb(nr);
            px_g(px_out) = (uint8_t) clamp_rgb(ng);
            px_b(px_out) = (uint8_t) clamp_rgb(nb);

        }
    }

    return out;
}


image imfilter_tile(image in, image out, int size) {
    if (size <= 1) {
        return imfilter_ident(in, out);
    }
    if (out == NULL) {
        out = im_new(in->width, in->height);
    }

    int tile_x, tile_y, x, y;
    for (tile_y = 0; tile_y * size < in->height; tile_y++) {
        for (tile_x = 0; tile_x * size < in->width; tile_x++) {
            uint64_t sr = 0, sg = 0, sb = 0;
            int n = 0;
            for (y = tile_y * size; y < (tile_y + 1) * size && y < in->height; y++) {
                for (x = tile_x * size; x < (tile_x + 1) * size && x < in->width; x++) {
                    pixel px = im_pixel(in, x, y);
                    sr += px_r(px);
                    sg += px_g(px);
                    sb += px_b(px);
                    n++;
                }
            }
            uint8_t r = (uint8_t) clamp_rgb(sr / n);
            uint8_t g = (uint8_t) clamp_rgb(sg / n);
            uint8_t b = (uint8_t) clamp_rgb(sb / n);
            for (y = tile_y * size; y < (tile_y + 1) * size && y < in->height; y++) {
                for (x = tile_x * size; x < (tile_x + 1) * size && x < in->width; x++) {
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

static image _halftone_error_diffusion(image in, image out, int n, int* dx, int* dy, double* w) {
    // invariant: in != out
    int x, y, i;
    double** error = (double **) malloc(sizeof(double *) * in->height);
    for (y = 0; y < in->height; y++) {
        error[y] = (double *) malloc(sizeof(double) * in->width);
        for (x = 0; x < in->width; x++) {
            error[y][x] = 0;
        }
    }

    for (y = 0; y < in->height; y++) {
        for (x = 0; x < in->width; x++) {
            pixel px = im_pixel(in, x, y);
            int g = _px_intensity(px);
            double ddark = (g - 0) + error[y][x];
            double dbright = (g - 255) + error[y][x];
            double e;
            px = im_pixel(out, x, y);
            if (fabs(ddark) < fabs(dbright)) {
                px_r(px) = 0;
                px_g(px) = 0;
                px_b(px) = 0;
                e = ddark;
            } else {
                px_r(px) = 255;
                px_g(px) = 255;
                px_b(px) = 255;
                e = dbright;
            }
            for (i = 0; i < n; i++) {
                int nx = x + dx[i];
                int ny = y + dy[i];
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

static image _halftone_ostromoukhov(image in, image out) {
    int dx[] = {1, -1, 0};
    int dy[] = {0, 1, 1};

    static double ostromoukhov_w[256][3];
    static int init_done = 0;

    if (!init_done) {
#define ostromoukhov_weight(idx, v0, v1, v2) ostromoukhov_w[idx][0] = v0, ostromoukhov_w[idx][1] = v1, ostromoukhov_w[idx][2] = v2;
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

        ostromoukhov_weight(0, 13.0 / 18.0, 0, 5.0 / 18.0);
        ostromoukhov_weight(1, 13.0 / 18.0, 0, 5.0 / 18.0);
        ostromoukhov_weight(2, 21.0 / 31.0, 0, 10.0 / 31.0);
        ostromoukhov_weight(3, 7.0 / 11.0, 0, 4.0 / 11.0);
        ostromoukhov_weight(4, 8.0 / 13.0, 0, 5.0 / 13.0);
        ostromoukhov_weight(10, 7.0 / 13.0, 3.0 / 13.0, 3.0 / 13.0);
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

        init_done = 1;

#undef ostromoukhov_weight
#undef ostromoukhov_interpolate
    }

    int x, y, i;
    double** error = (double **) malloc(sizeof(double *) * in->height);
    for (y = 0; y < in->height; y++) {
        error[y] = (double *) malloc(sizeof(double) * in->width);
        for (x = 0; x < in->width; x++) {
            error[y][x] = 0;
        }
    }

    for (y = 0; y < in->height; y++) {
        for (x = 0; x < in->width; x++) {
            pixel px = im_pixel(in, x, y);
            int g = _px_intensity(px);
            double ddark = (g - 0) + error[y][x];
            double dbright = (g - 255) + error[y][x];
            double e;
            px = im_pixel(out, x, y);
            if (fabs(ddark) < fabs(dbright)) {
                px_r(px) = 0;
                px_g(px) = 0;
                px_b(px) = 0;
                e = ddark;
            } else {
                px_r(px) = 255;
                px_g(px) = 255;
                px_b(px) = 255;
                e = dbright;
            }
            for (i = 0; i < 3; i++) {
                int nx = x + dx[i];
                int ny = y + dy[i];
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



image imfilter_halftone(image in, image out, int method) {
    int inplace = (in == out);
    if (out == NULL || inplace) {
        out = im_new(in->width, in->height);
    }

    switch (method) {
    case IMFILTER_SHIAU_FAN:
        {
            int n = 5;
            int dx[] = {-3, -2, -1, 0, 1};
            int dy[] = {1, 1, 1, 1, 0};
            double w[] = {0.0625, 0.0625, 0.125, 0.25, 0.5};
            _halftone_error_diffusion(in, out, n, dx, dy, w);
        }
        break;
    case IMFILTER_FLOYD_STEINBERG:
        {
            int n = 4;
            int dx[] = {-1, 0, 1, 1};
            int dy[] = {1, 1, 1, 0};
            double w[] = {0.1875, 0.3125, 0.0625, 0.4375};
            _halftone_error_diffusion(in, out, n, dx, dy, w);
        }
        break;
    case IMFILTER_OSTROMOUKHOV:
        _halftone_ostromoukhov(in, out);
        break;
    }

    if (inplace) {
        imfilter_ident(out, in);
        im_free(out);
        out = in;
    }
    return out;
}


image imfilter_box_blur(image in, image out, int radius) {
    if (radius <= 0) {
        return imfilter_ident(in, out);
    }
    int inplace = (in == out);
    if (out == NULL || inplace) {
        out = im_new(in->width, in->height);
    }

    uint64_t sr = 0, sg = 0, sb = 0;
    int x, y;
    int n = (2 * radius + 1) * (2 * radius + 1);

    // sweeping update, from top-left to bottom-right, zigzag path
    // update by subtracting pixels leaving brush range, and adding pixels joining brush range

    // initialize brush range
    for (y = -radius; y <= radius; y++) {
        for (x = -radius; x <= radius; x++) {
            pixel px = im_pixel_clamp(in, x, y);
            sr += px_r(px);
            sg += px_g(px);
            sb += px_b(px);
        }
    }

    y = 0;
    for (x = 0; x < in->width; x++) {
        int direction = (x & 0x1) ? -1 : 1;
        int cnt;
        for (cnt = 0; cnt < in->height; cnt++) {
            pixel px = im_pixel(out, x, y);
            px_r(px) = sr / n;
            px_g(px) = sg / n;
            px_b(px) = sb / n;

            int d;
            if (cnt < in->height - 1) {
                // sweep horizontally
                int ny = y - direction * radius;
                for (d = -radius; d <= radius; d++) {
                    px = im_pixel_clamp(in, x + d, ny);
                    sr -= px_r(px);
                    sg -= px_g(px);
                    sb -= px_b(px);
                }
                y += direction;
                ny = y + direction * radius;
                for (d = -radius; d <= radius; d++) {
                    px = im_pixel_clamp(in, x + d, ny);
                    sr += px_r(px);
                    sg += px_g(px);
                    sb += px_b(px);
                }
            } else {
                // sweep to the right
                for (d = -radius; d <= radius; d++) {
                    px = im_pixel_clamp(in, x - radius, y + d);
                    sr -= px_r(px);
                    sg -= px_g(px);
                    sb -= px_b(px);
                }
                for (d = -radius; d <= radius; d++) {
                    px = im_pixel_clamp(in, x + radius + 1, y + d);
                    sr += px_r(px);
                    sg += px_g(px);
                    sb += px_b(px);
                }
            }
        }
    }

    if (inplace) {
        imfilter_ident(out, in);
        im_free(out);
        out = in;
    }
    return out;
}


image imfilter_gaussian_blur(image in, image out, int radius) {
    // this is not exact gaussian, but result is similar, and performance is better
    image tmp = im_dup(in);
    out = imfilter_box_blur(tmp, out, radius / 3);
    imfilter_box_blur(out, tmp, radius / 3);
    imfilter_box_blur(tmp, out, radius - 2 * (radius / 3));
    im_free(tmp);
    return out;
}


image imfilter_conv(image in, image out, int n, int* dx, int* dy, double* w, uint8_t b) {
    if (n <= 0) {
        return imfilter_ident(in, out);
    }
    int inplace = (in == out);
    if (out == NULL || inplace) {
        out = im_new(in->width, in->height);
    }

    int x, y, i;
    for (y = 0; y < in->height; y++) {
        for (x = 0; x < in->width; x++) {
            pixel px_out = im_pixel(out, x, y);
            double vr = b, vg = b, vb = b;
            for (i = 0; i < n; i++) {
                int nx = x + dx[i];
                int ny = y + dy[i];
                pixel px = im_pixel_clamp(in, nx, ny);
                vr += w[i] * px_r(px);
                vg += w[i] * px_g(px);
                vb += w[i] * px_b(px);
            }
            px_r(px_out) = (uint8_t) clamp_rgb(vr);
            px_g(px_out) = (uint8_t) clamp_rgb(vg);
            px_b(px_out) = (uint8_t) clamp_rgb(vb);
        }
    }

    if (inplace) {
        imfilter_ident(out, in);
        im_free(out);
        out = in;
    }
    return out;
}


image imfilter_pencil(image in, image out, double strength) {
    image tmp = imfilter_box_blur(in, NULL, 1);
    imfilter_box_blur(tmp, out, 1);
    imfilter_box_blur(out, tmp, 1);
    int n = 5;
    int dx[] = {0, -1, 1, 0, 0};
    int dy[] = {0, 0, 0, -1, 1};
    double w[] = {4 * strength, -strength, -strength, -strength, -strength};
    imfilter_conv(tmp, out, n, dx, dy, w, 255);
    im_free(tmp);
    return out;
}

static int _level_scale(int v, int levels) {
    int r = v * levels + 128;
    r = ((r >> 8) + r) >> 8;
    return r;
}

image imfilter_oil_paint(image in, image out, int radius, int details) {
    if (radius <= 0) {
        return imfilter_ident(in, out);
    }
    int inplace = (in == out);
    if (out == NULL || inplace) {
        out = im_new(in->width, in->height);
    }

    int levels = 1 + details;
    int* intensity_count = (int *) malloc(sizeof(int) * levels);
    memset(intensity_count, 0, sizeof(int) * levels);
    uint64_t* sr = (uint64_t *) malloc(sizeof(uint64_t) * levels);
    memset(sr, 0, sizeof(uint64_t) * levels);
    uint64_t* sg = (uint64_t *) malloc(sizeof(uint64_t) * levels);
    memset(sg, 0, sizeof(uint64_t) * levels);
    uint64_t* sb = (uint64_t *) malloc(sizeof(uint64_t) * levels);
    memset(sb, 0, sizeof(uint64_t) * levels);

    int x, y;

    for (y = -radius; y <= radius; y++) {
        for (x = -radius; x <= radius; x++) {
            pixel px = im_pixel_clamp(in, x, y);
            int intensity = _px_intensity(px);
            int intensity_level = _level_scale(intensity, details);
            intensity_count[intensity_level]++;
            sr[intensity_level] += px_r(px);
            sg[intensity_level] += px_g(px);
            sb[intensity_level] += px_b(px);
        }
    }

    y = 0;
    for (x = 0; x < in->width; x++) {
        int direction = (x & 0x1) ? -1 : 1;
        int cnt;
        for (cnt = 0; cnt < in->height; cnt++) {
            int i, max_idx = 0;
            for (i = 1; i < levels; i++) {
                if (intensity_count[i] > intensity_count[max_idx]) {
                    max_idx = i;
                }
            }

            double vr = 1.0 * sr[max_idx] / intensity_count[max_idx];
            uint8_t r = (uint8_t) clamp_rgb(vr);
            double vg = 1.0 * sg[max_idx] / intensity_count[max_idx];
            uint8_t g = (uint8_t) clamp_rgb(vg);
            double vb = 1.0 * sb[max_idx] / intensity_count[max_idx];
            uint8_t b = (uint8_t) clamp_rgb(vb);

            pixel px = im_pixel(out, x, y);
            px_r(px) = r;
            px_g(px) = g;
            px_b(px) = b;

            int d;
            if (cnt < in->height - 1) {
                int ny = y - direction * radius;
                for (d = -radius; d <= radius; d++) {
                    px = im_pixel_clamp(in, x + d, ny);
                    int intensity = _px_intensity(px);
                    int intensity_level = _level_scale(intensity, details);
                    intensity_count[intensity_level]--;
                    sr[intensity_level] -= px_r(px);
                    sg[intensity_level] -= px_g(px);
                    sb[intensity_level] -= px_b(px);
                }
                y += direction;
                ny = y + direction * radius;
                for (d = -radius; d <= radius; d++) {
                    px = im_pixel_clamp(in, x + d, ny);
                    int intensity = _px_intensity(px);
                    int intensity_level = _level_scale(intensity, details);
                    intensity_count[intensity_level]++;
                    sr[intensity_level] += px_r(px);
                    sg[intensity_level] += px_g(px);
                    sb[intensity_level] += px_b(px);
                }
            } else {
                for (d = -radius; d <= radius; d++) {
                    px = im_pixel_clamp(in, x - radius, y + d);
                    int intensity = _px_intensity(px);
                    int intensity_level = _level_scale(intensity, details);
                    intensity_count[intensity_level]--;
                    sr[intensity_level] -= px_r(px);
                    sg[intensity_level] -= px_g(px);
                    sb[intensity_level] -= px_b(px);
                }
                for (d = -radius; d <= radius; d++) {
                    px = im_pixel_clamp(in, x + radius + 1, y + d);
                    int intensity = _px_intensity(px);
                    int intensity_level = _level_scale(intensity, details);
                    intensity_count[intensity_level]++;
                    sr[intensity_level] += px_r(px);
                    sg[intensity_level] += px_g(px);
                    sb[intensity_level] += px_b(px);

                }
            }
        }
    }

    free(intensity_count);
    free(sr);
    free(sg);
    free(sb);

    if (inplace) {
        imfilter_ident(out, in);
        im_free(out);
        out = in;
    }
    return out;
}


image imfilter_dilate(image in, image out, int radius) {
    if (radius <= 0) {
        return imfilter_ident(in, out);
    }
    int inplace = (in == out);
    if (out == NULL || inplace) {
        out = im_new(in->width, in->height);
    }

    int x, y;
    int* r_count = (int *) malloc(sizeof(int) * 256);
    memset(r_count, 0, sizeof(int) * 256);
    int* g_count = (int *) malloc(sizeof(int) * 256);
    memset(g_count, 0, sizeof(int) * 256);
    int* b_count = (int *) malloc(sizeof(int) * 256);
    memset(b_count, 0, sizeof(int) * 256);

    for (y = -radius; y <= radius; y++) {
        for (x = -radius; x <= radius; x++) {
            pixel px = im_pixel_clamp(in, x, y);
            r_count[px_r(px)]++;
            g_count[px_g(px)]++;
            b_count[px_b(px)]++;
        }
    }

    y = 0;
    for (x = 0; x < in->width; x++) {
        int direction = (x & 0x1) ? -1 : 1;
        int cnt;
        for (cnt = 0; cnt < in->height; cnt++) {
            pixel px = im_pixel(out, x, y);
            int i;
            for (i = 255; i >= 0; i--) {
                if (r_count[i] != 0) {
                    px_r(px) = i;
                    break;
                }
            }
            for (i = 255; i >= 0; i--) {
                if (g_count[i] != 0) {
                    px_g(px) = i;
                    break;
                }
            }
            for (i = 255; i >= 0; i--) {
                if (b_count[i] != 0) {
                    px_b(px) = i;
                    break;
                }
            }

            int d;
            if (cnt < in->height - 1) {
                // sweep horizontally
                int ny = y - direction * radius;
                for (d = -radius; d <= radius; d++) {
                    px = im_pixel_clamp(in, x + d, ny);
                    r_count[px_r(px)]--;
                    g_count[px_g(px)]--;
                    b_count[px_b(px)]--;
                }
                y += direction;
                ny = y + direction * radius;
                for (d = -radius; d <= radius; d++) {
                    px = im_pixel_clamp(in, x + d, ny);
                    r_count[px_r(px)]++;
                    g_count[px_g(px)]++;
                    b_count[px_b(px)]++;
                }
            } else {
                // sweep to the right
                for (d = -radius; d <= radius; d++) {
                    px = im_pixel_clamp(in, x - radius, y + d);
                    r_count[px_r(px)]--;
                    g_count[px_g(px)]--;
                    b_count[px_b(px)]--;
                }
                for (d = -radius; d <= radius; d++) {
                    px = im_pixel_clamp(in, x + radius + 1, y + d);
                    r_count[px_r(px)]++;
                    g_count[px_g(px)]++;
                    b_count[px_b(px)]++;
                }
            }
        }
    }

    if (inplace) {
        imfilter_ident(out, in);
        im_free(out);
        out = in;
    }
    return out;
}

image imfilter_erode(image in, image out, int radius) {
    if (radius <= 0) {
        return imfilter_ident(in, out);
    }
    int inplace = (in == out);
    if (out == NULL || inplace) {
        out = im_new(in->width, in->height);
    }

    int x, y;
    int* r_count = (int *) malloc(sizeof(int) * 256);
    memset(r_count, 0, sizeof(int) * 256);
    int* g_count = (int *) malloc(sizeof(int) * 256);
    memset(g_count, 0, sizeof(int) * 256);
    int* b_count = (int *) malloc(sizeof(int) * 256);
    memset(b_count, 0, sizeof(int) * 256);

    for (y = -radius; y <= radius; y++) {
        for (x = -radius; x <= radius; x++) {
            pixel px = im_pixel_clamp(in, x, y);
            r_count[px_r(px)]++;
            g_count[px_g(px)]++;
            b_count[px_b(px)]++;
        }
    }

    y = 0;
    for (x = 0; x < in->width; x++) {
        int direction = (x & 0x1) ? -1 : 1;
        int cnt;
        for (cnt = 0; cnt < in->height; cnt++) {
            pixel px = im_pixel(out, x, y);
            int i;
            for (i = 0; i <= 255; i++) {
                if (r_count[i] != 0) {
                    px_r(px) = i;
                    break;
                }
            }
            for (i = 0; i <= 255; i++) {
                if (g_count[i] != 0) {
                    px_g(px) = i;
                    break;
                }
            }
            for (i = 0; i <= 255; i++) {
                if (b_count[i] != 0) {
                    px_b(px) = i;
                    break;
                }
            }

            int d;
            if (cnt < in->height - 1) {
                // sweep horizontally
                int ny = y - direction * radius;
                for (d = -radius; d <= radius; d++) {
                    px = im_pixel_clamp(in, x + d, ny);
                    r_count[px_r(px)]--;
                    g_count[px_g(px)]--;
                    b_count[px_b(px)]--;
                }
                y += direction;
                ny = y + direction * radius;
                for (d = -radius; d <= radius; d++) {
                    px = im_pixel_clamp(in, x + d, ny);
                    r_count[px_r(px)]++;
                    g_count[px_g(px)]++;
                    b_count[px_b(px)]++;
                }
            } else {
                // sweep to the right
                for (d = -radius; d <= radius; d++) {
                    px = im_pixel_clamp(in, x - radius, y + d);
                    r_count[px_r(px)]--;
                    g_count[px_g(px)]--;
                    b_count[px_b(px)]--;
                }
                for (d = -radius; d <= radius; d++) {
                    px = im_pixel_clamp(in, x + radius + 1, y + d);
                    r_count[px_r(px)]++;
                    g_count[px_g(px)]++;
                    b_count[px_b(px)]++;
                }
            }
        }
    }

    if (inplace) {
        imfilter_ident(out, in);
        im_free(out);
        out = in;
    }
    return out;
}


image imfilter_opening(image in, image out, int radius) {
    if (radius <= 0) {
        return imfilter_ident(in, out);
    }
    image tmp = imfilter_erode(in, NULL, radius);
    imfilter_dilate(tmp, out, radius);
    im_free(tmp);
    return out;
}

image imfilter_closing(image in, image out, int radius) {
    if (radius <= 0) {
        return imfilter_ident(in, out);
    }
    image tmp = imfilter_dilate(in, NULL, radius);
    imfilter_erode(tmp, out, radius);
    im_free(tmp);
    return out;
}

image imfilter_watercolor_dark(image in, image out, int radius) {
    if (radius <= 0) {
        return imfilter_ident(in, out);
    }
    // opening -> closing
    image tmp = imfilter_erode(in, NULL, radius);
    imfilter_dilate(tmp, out, radius);

    imfilter_dilate(out, tmp, radius);
    imfilter_erode(tmp, out, radius);

    return out;
}

image imfilter_watercolor_bright(image in, image out, int radius) {
    if (radius <= 0) {
        return imfilter_ident(in, out);
    }
    // closing -> opening
    image tmp = imfilter_dilate(in, NULL, radius);
    imfilter_erode(tmp, out, radius);

    imfilter_erode(out, tmp, radius);
    imfilter_dilate(tmp, out, radius);

    return out;
}
