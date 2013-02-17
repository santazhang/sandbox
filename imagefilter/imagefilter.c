#include "imagefilter.h"

#define _limit(c) ((c) < 0 ? 0 : ((c) > 255 ? 255 : (c)))

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

image im_inv(image in, image out) {
    int inplace = (in == out);
    if (out == NULL || inplace) {
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

image im_gray(image in, image out, double r_weight, double g_weight, double b_weight) {
    int inplace = (in == out);
    if (out == NULL || inplace) {
        out = im_new(in->width, in->height);
    }
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

image im_contrast(image in, image out, double slope) {
    int inplace = (in == out);
    if (out == NULL || inplace) {
        out = im_new(in->width, in->height);
    }
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

image im_tile(image in, image out, int tile_size) {
    int inplace = (in == out);
    if (out == NULL || inplace) {
        out = im_new(in->width, in->height);
    }
    
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


image im_avg_blur(image in, image out, double radius) {
    int inplace = (in == out);
    if (out == NULL || inplace) {
        out = im_new(in->width, in->height);
    }
    
    // TODO diff based O(n^2) algorithm
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
    image tmp = im_new(in->width, in->height);
    im_opening(in, tmp, radius);
    im_closing(tmp, out, radius);
    im_free(tmp);
    return out;
}

image im_watercolor_bright(image in, image out, double radius) {
    // closing -> opening
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
    im_inv(tmp, out);
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