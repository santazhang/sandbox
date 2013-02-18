#include <assert.h>
#include <stdio.h>

#include <sys/time.h>
#include <png.h>

#include "imfilter.h"
#include "impng.h"

static struct timeval t_start, t_stop;

void timer_start() {
    gettimeofday(&t_start, NULL);
}

double timer_stop() {
    gettimeofday(&t_stop, NULL);
    double sec = t_stop.tv_sec - t_start.tv_sec + (t_stop.tv_usec - t_start.tv_usec) / 1000000.0;
    return sec;
}

int main(int argc, char* argv[]) {
    image src = impng_load("t.png");
    int pixels = src->width * src->height;
    double sec;

    timer_start();
    image test = imfilter_ident(src, NULL);
    sec = timer_stop();
    printf("ident: %.2lf million pixels per second\n", pixels / sec / 1000000.0);
    impng_save(test, "t-ident.png");

    timer_start();
    imfilter_inv(src, test);
    sec = timer_stop();
    printf("inv: %.2lf million pixels per second\n", pixels / sec / 1000000.0);
    impng_save(test, "t-inv.png");

    imfilter_ident(src, test);
    timer_start();
    imfilter_intensity(test, test);
    sec = timer_stop();
    printf("intensity: %.2lf million pixels per second\n", pixels / sec / 1000000.0);
    impng_save(test, "t-intensity.png");

    imfilter_ident(src, test);
    timer_start();
    imfilter_pencil(test, test, 10.0);
    sec = timer_stop();
    printf("pencil: %.2lf million pixels per second\n", pixels / sec / 1000000.0);
    impng_save(test, "t-pencil.png");

    imfilter_ident(src, test);
    timer_start();
    imfilter_oil_paint(test, test, 10, 5);
    sec = timer_stop();
    printf("oil-paint: %.2lf million pixels per second\n", pixels / sec / 1000000.0);
    impng_save(test, "t-oil-paint.png");

    imfilter_ident(src, test);
    timer_start();
    imfilter_dilate(test, test, 2);
    sec = timer_stop();
    printf("dilate: %.2lf million pixels per second\n", pixels / sec / 1000000.0);
    impng_save(test, "t-dilate.png");

    imfilter_ident(src, test);
    timer_start();
    imfilter_erode(test, test, 2);
    sec = timer_stop();
    printf("erode: %.2lf million pixels per second\n", pixels / sec / 1000000.0);
    impng_save(test, "t-erode.png");

    imfilter_ident(src, test);
    timer_start();
    imfilter_opening(test, test, 2);
    sec = timer_stop();
    printf("opening: %.2lf million pixels per second\n", pixels / sec / 1000000.0);
    impng_save(test, "t-opening.png");

    imfilter_ident(src, test);
    timer_start();
    imfilter_closing(test, test, 2);
    sec = timer_stop();
    printf("closing: %.2lf million pixels per second\n", pixels / sec / 1000000.0);
    impng_save(test, "t-closing.png");

    imfilter_ident(src, test);
    timer_start();
    imfilter_watercolor_dark(test, test, 5);
    sec = timer_stop();
    printf("watercolor-dark: %.2lf million pixels per second\n", pixels / sec / 1000000.0);
    impng_save(test, "t-watercolor-dark.png");

    imfilter_ident(src, test);
    timer_start();
    imfilter_watercolor_bright(test, test, 5);
    sec = timer_stop();
    printf("watercolor-bright: %.2lf million pixels per second\n", pixels / sec / 1000000.0);
    impng_save(test, "t-watercolor-bright.png");

    imfilter_ident(src, test);
    timer_start();
    imfilter_contrast(test, test, 1.5);
    sec = timer_stop();
    printf("contrast: %.2lf million pixels per second\n", pixels / sec / 1000000.0);
    impng_save(test, "t-contrast.png");

    imfilter_ident(src, test);
    timer_start();
    imfilter_tile(test, test, 20);
    sec = timer_stop();
    printf("tile: %.2lf million pixels per second\n", pixels / sec / 1000000.0);
    impng_save(test, "t-tile.png");

    imfilter_ident(src, test);
    timer_start();
    imfilter_box_blur(test, test, 10);
    sec = timer_stop();
    printf("box-blur: %.2lf million pixels per second\n", pixels / sec / 1000000.0);
    impng_save(test, "t-blur-box.png");

    imfilter_ident(src, test);
    timer_start();
    imfilter_gaussian_blur(test, test, 10);
    sec = timer_stop();
    printf("gaussian-blur: %.2lf million pixels per second\n", pixels / sec / 1000000.0);
    impng_save(test, "t-blur-gaussian.png");

    imfilter_ident(src, test);
    timer_start();
    imfilter_halftone(test, test, IMFILTER_SHIAU_FAN);
    sec = timer_stop();
    printf("halftone-shiau-fan: %.2lf million pixels per second\n", pixels / sec / 1000000.0);
    impng_save(test, "t-halftone-shiau-fan.png");

    imfilter_ident(src, test);
    timer_start();
    imfilter_halftone(test, test, IMFILTER_FLOYD_STEINBERG);
    sec = timer_stop();
    printf("halftone-floyd-steinberg: %.2lf million pixels per second\n", pixels / sec / 1000000.0);
    impng_save(test, "t-halftone-floyd-steinberg.png");

    timer_start();
    imfilter_halftone(src, test, IMFILTER_OSTROMOUKHOV);
    sec = timer_stop();
    printf("halftone-ostromoukhov: %.2lf million pixels per second\n", pixels / sec / 1000000.0);
    impng_save(test, "t-halftone-ostromoukhov.png");

    im_free(src);
    im_free(test);
    return 0;
}
