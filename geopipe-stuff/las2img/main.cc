#include <stdio.h>
#include <math.h>
#include <inttypes.h>
#include <string.h>

#include <string>

#include <laslib/lasreader.hpp>

#include "utils/inc_gflags.h"

using std::string;

DEFINE_double(stddev_cap, 20.0, "stddev above this value will be truncated");
DEFINE_double(resolution, 0.15, "meters per pixel on output image");
DEFINE_double(stddev_box_radius_in_meters, 1.0, "radius of neighbor box for stddev sampling");

// hsv <-> rgb from http://stackoverflow.com/a/6930407/1035246
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

hsv rgb2hsv(rgb in)
{
    hsv         out;
    double      min, max, delta;

    min = in.r < in.g ? in.r : in.g;
    min = min  < in.b ? min  : in.b;

    max = in.r > in.g ? in.r : in.g;
    max = max  > in.b ? max  : in.b;

    out.v = max;                                // v
    delta = max - min;
    if (delta < 0.00001)
    {
        out.s = 0;
        out.h = 0; // undefined, maybe nan?
        return out;
    }
    if( max > 0.0 ) { // NOTE: if Max is == 0, this divide would cause a crash
        out.s = (delta / max);                  // s
    } else {
        // if max is 0, then r = g = b = 0
            // s = 0, v is undefined
        out.s = 0.0;
        out.h = NAN;                            // its now undefined
        return out;
    }
    if( in.r >= max )                           // > is bogus, just keeps compilor happy
        out.h = ( in.g - in.b ) / delta;        // between yellow & magenta
    else
    if( in.g >= max )
        out.h = 2.0 + ( in.b - in.r ) / delta;  // between cyan & yellow
    else
        out.h = 4.0 + ( in.r - in.g ) / delta;  // between magenta & cyan

    out.h *= 60.0;                              // degrees

    if( out.h < 0.0 )
        out.h += 360.0;

    return out;
}


rgb hsv2rgb(hsv in)
{
    double      hh, p, q, t, ff;
    long        i;
    rgb         out;

    if(in.s <= 0.0) {       // < is bogus, just shuts up warnings
        out.r = in.v;
        out.g = in.v;
        out.b = in.v;
        return out;
    }
    hh = in.h;
    if(hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = in.v * (1.0 - in.s);
    q = in.v * (1.0 - (in.s * ff));
    t = in.v * (1.0 - (in.s * (1.0 - ff)));

    switch(i) {
    case 0:
        out.r = in.v;
        out.g = t;
        out.b = p;
        break;
    case 1:
        out.r = q;
        out.g = in.v;
        out.b = p;
        break;
    case 2:
        out.r = p;
        out.g = in.v;
        out.b = t;
        break;

    case 3:
        out.r = p;
        out.g = q;
        out.b = in.v;
        break;
    case 4:
        out.r = t;
        out.g = p;
        out.b = in.v;
        break;
    case 5:
    default:
        out.r = in.v;
        out.g = p;
        out.b = q;
        break;
    }
    return out;
}

inline int quantize(double value, double lb, double ub, int levels) {
    return nearbyint((levels - 1) * (value - lb) / (ub - lb));
}

int main(int argc, char* argv[]) {
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    if (argc < 2) {
        printf("Usage: %s <las-filename>\n", argv[0]);
        exit(1);
    }

    LASreadOpener lasreadopener;
    std::string las_fn = argv[1];
    lasreadopener.set_file_name(las_fn.c_str());
    LASreader* lasreader = lasreadopener.open();
    printf("x: %lf ~ %lf, y: %lf ~ %lf, z: %lf ~ %lf\n",
           lasreader->get_min_x(), lasreader->get_max_x(),
           lasreader->get_min_y(), lasreader->get_max_y(),
           lasreader->get_min_z(), lasreader->get_max_z());

    double min_x = lasreader->get_min_x();
    double max_x = lasreader->get_max_x();
    double min_y = lasreader->get_min_y();
    double max_y = lasreader->get_max_y();
    double min_z = lasreader->get_min_z();
    double max_z = lasreader->get_max_z();
    int img_width = ceilf((max_x - min_x) / FLAGS_resolution);
    int img_height = ceilf((max_y - min_y) / FLAGS_resolution);
    printf("Image size: %d x %d\n", img_width, img_height);

    uint8_t* img = new uint8_t[img_width * img_height];
    memset(img, 0, img_width * img_height);

    printf("Reading %lld points\n", lasreader->npoints);
    while (lasreader->read_point()) {
        const LASpoint& p = lasreader->point;
        int x = quantize(p.get_x(), min_x, max_x, img_width);
        int y = img_height - 1 - quantize(p.get_y(), min_y, max_y, img_height);
        int z = 1 + quantize(p.get_z(), min_z, max_z, 255);
        int idx = img_height * y + x;
        img[idx] = z;
    }
    lasreader->close();

    printf("Writing ppm file\n");
    FILE* fp = fopen((las_fn + ".decoded.z.ppm").c_str(), "wb");
    fprintf(fp, "P6\n%d %d\n255\n", img_width, img_height);
    for (int i = 0; i < img_width * img_height; i++) {
        uint8_t v = img[i];
        if (v == 0) {
            fwrite(&v, 1, 1, fp);
            fwrite(&v, 1, 1, fp);
            fwrite(&v, 1, 1, fp);
        } else {
            hsv h;
            h.h = img[i] * 360.0 / 256.0;
            h.s = 1;
            h.v = 1;
            rgb c = hsv2rgb(h);
            uint8_t r = nearbyint(255 * c.r);
            uint8_t g = nearbyint(255 * c.g);
            uint8_t b = nearbyint(255 * c.b);
            fwrite(&r, 1, 1, fp);
            fwrite(&g, 1, 1, fp);
            fwrite(&b, 1, 1, fp);
        }
    }
    fclose(fp);

    printf("Writing stddev ppm file\n");
    fp = fopen((las_fn + ".decoded.z_stddev.ppm").c_str(), "wb");
    fprintf(fp, "P6\n%d %d\n255\n", img_width, img_height);
    double* hue_img1 = new double[img_width * img_height];
    for (int i = 0; i < img_width * img_height; i++) {
        hue_img1[i] = 0;
    }
    const int radius = floor(FLAGS_stddev_box_radius_in_meters / FLAGS_resolution + 1);
    uint8_t* samples = new uint8_t[(2 * radius + 1) * (2 * radius + 1)];
    int pct = 0;
    for (int i = 0; i < img_width * img_height; i++) {
        if (i % img_width == 0) {
            int new_pct = 100.0 * i / img_width / img_height;
            if (new_pct != pct) {
                printf("%d%% done\n", new_pct);
                pct = new_pct;
            }
        }
        uint8_t v = img[i];
        if (v == 0) {
            fwrite(&v, 1, 1, fp);
            fwrite(&v, 1, 1, fp);
            fwrite(&v, 1, 1, fp);
        } else {
            hsv h;
            {
                // TODO enhance stddev calculation with moving window, reuse calculated data
                // stddev(x) = sqrt(E(x^2) - (E(x))^2)
                int n_samples = 0;
                int cur_y = i / img_width;
                int cur_x = i - cur_y * img_width;
                int x1 = cur_x - radius;
                if (x1 < 0) {
                    x1 = 0;
                }
                int x2 = cur_x + radius;
                if (x2 >= img_width) {
                    x2 = img_width - 1;
                }
                int y1 = cur_y - radius;
                if (y1 < 0) {
                    y1 = 0;
                }
                int y2 = cur_y + radius;
                if (y2 >= img_height) {
                    y2 = img_height - 1;
                }
                for (int x = x1; x <= x2; x++) {
                    for (int y = y1; y <= y2; y++) {
                        int idx = y * img_width + x;
                        if (img[idx] != 0) {
                            samples[n_samples] = img[idx];
                            n_samples++;
                        }
                    }
                }
                double sum = 0.0;
                for (int j = 0; j < n_samples; j++) {
                    sum += samples[j];
                }
                double mean = sum / n_samples;
                double sum2 = 0.0;
                for (int j = 0; j < n_samples; j++) {
                    sum2 += (samples[j] - mean) * (samples[j] - mean);
                }
                double stddev = sqrt(sum2 / n_samples);
                h.h = (stddev / FLAGS_stddev_cap) * 300;  // don't go back to 0 degrees (red)
                if (h.h > 300.0) {
                    h.h = 300.0;
                }
                hue_img1[i] = h.h;
            }
            h.s = 1;
            h.v = 1;
            rgb c = hsv2rgb(h);
            uint8_t r = nearbyint(255 * c.r);
            uint8_t g = nearbyint(255 * c.g);
            uint8_t b = nearbyint(255 * c.b);
            fwrite(&r, 1, 1, fp);
            fwrite(&g, 1, 1, fp);
            fwrite(&b, 1, 1, fp);
        }
    }
    fclose(fp);

    fp = fopen((las_fn + ".decoded.z_stddev_gaussian.ppm").c_str(), "wb");
    fprintf(fp, "P6\n%d %d\n255\n", img_width, img_height);
    double* gaussian_weight = new double[(radius + 1) * (radius + 1)];
    printf("Gaussian weight:\n");
    for (int i = 0; i <= radius; i++) {
        for (int j = 0; j <= radius; j++) {
            int idx = i * (radius + 1) + j;
            gaussian_weight[idx] = exp(-0.5*(i*i + j*j)/(radius*radius) * 4);
            printf("%lf ", gaussian_weight[idx]);
        }
        printf("\n");
    }
    pct = 0;
    for (int cur_y = 0; cur_y < img_height; cur_y++) {
        int new_pct = 100.0 * cur_y / img_height;
        if (new_pct != pct) {
            printf("%d%% done\n", new_pct);
            pct = new_pct;
        }
        for (int cur_x = 0; cur_x < img_width; cur_x++) {
            int x1 = cur_x - radius;
            if (x1 < 0) {
                x1 = 0;
            }
            int x2 = cur_x + radius;
            if (x2 >= img_width) {
                x2 = img_width - 1;
            }
            int y1 = cur_y - radius;
            if (y1 < 0) {
                y1 = 0;
            }
            int y2 = cur_y + radius;
            if (y2 >= img_height) {
                y2 = img_height - 1;
            }
            double hue_sum = 0.0;
            int n_samples = 0;
            double hue_weight_sum = 0.0;
            for (int x = x1; x <= x2; x++) {
                for (int y = y1; y <= y2; y++) {
                    int idx = y * img_width + x;
                    if (hue_img1[idx] > 1e-6) {
                        int dx = abs(x - cur_x);
                        int dy = abs(y - cur_y);
                        int idx_weight = dy * (1 + radius) + dx;
                        hue_weight_sum += gaussian_weight[idx_weight];
                        hue_sum += hue_img1[idx] * gaussian_weight[idx_weight];
                        samples[n_samples] = img[idx];
                        n_samples++;
                    }
                }
            }
            // printf("nsampels = %d hue-sum=%lf hue-weights=%lf\n", n_samples, hue_sum, hue_weight_sum);
            if (n_samples == 0) {
                uint8_t black = 0;
                fwrite(&black, 1, 1, fp);
                fwrite(&black, 1, 1, fp);
                fwrite(&black, 1, 1, fp);
            } else {
                hsv h;
                h.h = hue_sum / hue_weight_sum;
                // printf("hue=%lf\n", h.h);
                h.s = 1;
                h.v = 1;
                rgb c = hsv2rgb(h);
                uint8_t r = nearbyint(255 * c.r);
                uint8_t g = nearbyint(255 * c.g);
                uint8_t b = nearbyint(255 * c.b);
                fwrite(&r, 1, 1, fp);
                fwrite(&g, 1, 1, fp);
                fwrite(&b, 1, 1, fp);
            }
        }
    }
    fclose(fp);

    delete lasreader;
    delete[] img;
    delete[] hue_img1;
    delete[] samples;
    return 0;
}
