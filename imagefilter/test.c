#include <assert.h>
#include <stdio.h>

#include <unistd.h>
#include <png.h>

#include "imagefilter.h"


int main(int argc, char* argv[]) {
    char* in = "t.png";
    char* out = "t-out.png";

    int width, height;
    png_structp png_ptr;
    png_infop info_ptr;

    // read png
    char header[8];
    FILE *fp = fopen(in, "rb");
    assert(fp);
    fread(header, 1, 8, fp);
    assert(png_sig_cmp(header, 0, 8) == 0);
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    assert(png_ptr);
    info_ptr = png_create_info_struct(png_ptr);
    assert(info_ptr);
    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8);
    png_read_info(png_ptr, info_ptr);
    assert(png_get_color_type(png_ptr, info_ptr) == PNG_COLOR_TYPE_RGB);
    width = png_get_image_width(png_ptr, info_ptr);
    height = png_get_image_height(png_ptr, info_ptr);
    image im = im_new(width, height);
    png_read_image(png_ptr, im->data);
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    fclose(fp);

    // process filter
    struct timeval begin, end;
    gettimeofday(&begin, NULL);
    im_halftone_ostromoukhov(im, im);
    gettimeofday(&end, NULL);
    double sec = end.tv_sec - begin.tv_sec + (end.tv_usec - begin.tv_usec) / 1000000.0;
    printf("%.2lf M pixels per second\n", im->width * im->height / sec / 1000000);

    // write png
    fp = fopen(out, "wb");
    assert(fp);
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    assert(png_ptr);
    info_ptr = png_create_info_struct(png_ptr);
    assert(info_ptr);
    png_init_io(png_ptr, fp);
    int bit_depth = 8;
    png_set_IHDR(png_ptr, info_ptr, im->width, im->height,
                 bit_depth, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
    png_write_info(png_ptr, info_ptr);
    png_write_image(png_ptr, im->data);
    png_write_end(png_ptr, NULL);
    fclose(fp);
    png_destroy_write_struct(&png_ptr, &info_ptr);

    // free resource
    im_free(im);
    return 0;
}
