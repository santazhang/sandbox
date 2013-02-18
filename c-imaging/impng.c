#include <png.h>

#include "impng.h"

image impng_load(const char* path) {
    image im = NULL;
    int width, height;
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    unsigned char header[8];

    FILE *fp = fopen(path, "rb");
    if (fp == NULL) {
        goto out;
    }
    fread(header, 1, 8, fp);
    if (!(png_sig_cmp(header, 0, 8) == 0)) {
        goto out;
    }
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL) {
        goto out;
    }
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
        goto out;
    }
    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8);
    png_read_info(png_ptr, info_ptr);
    if (png_get_color_type(png_ptr, info_ptr) != PNG_COLOR_TYPE_RGB) {
        goto out;
    }
    width = png_get_image_width(png_ptr, info_ptr);
    height = png_get_image_height(png_ptr, info_ptr);
    im = im_new(width, height);
    png_read_image(png_ptr, im->data);
out:
    if (png_ptr != NULL) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    }
    if (fp != NULL) {
        fclose(fp);
    }
    return im;
}

int impng_save(image im, const char* path) {
    int r = -1;
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;

    FILE* fp = fopen(path, "wb");
    if (fp == NULL) {
        goto out;
    }
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL) {
        goto out;
    }
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
        goto out;
    }
    png_init_io(png_ptr, fp);
    png_set_IHDR(png_ptr, info_ptr, im->width, im->height,
                 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
    png_write_info(png_ptr, info_ptr);
    png_write_image(png_ptr, im->data);
    png_write_end(png_ptr, NULL);
    r = 0;
out:
    if (png_ptr != NULL) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
    }
    if (fp != NULL) {
        fclose(fp);
    }
    return r;
}
