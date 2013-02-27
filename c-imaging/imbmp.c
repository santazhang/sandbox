#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "imbmp.h"

int imbmp_save(image im, const char* path) {
    int r = -1;
    uint8_t* bgr_row = NULL;

    FILE* fp = fopen(path, "wb");
    if (fp == NULL) {
        goto out;
    }

    uint8_t bmp_file_header[14];
    uint8_t bmp_info_header[40];
    const int data_offset = sizeof(bmp_file_header) + sizeof(bmp_info_header);
    int file_size = im->width * im->height * 3 + data_offset;

    memset(bmp_file_header, 0, sizeof(bmp_file_header));
    bmp_file_header[0] = 'B';
    bmp_file_header[1] = 'M';
    *((int32_t *) &(bmp_file_header[2])) = (int32_t) file_size;
    *((int32_t *) &(bmp_file_header[10])) = (int32_t) data_offset;

    memset(bmp_info_header, 0, sizeof(bmp_info_header));
    *((int32_t *) &(bmp_info_header[0])) = (int32_t) sizeof(bmp_info_header);
    *((int32_t *) &(bmp_info_header[4])) = (int32_t) im->width;
    *((int32_t *) &(bmp_info_header[8])) = (int32_t) im->height;
    *((int16_t *) &(bmp_info_header[12])) = (int16_t) 1;  // number of color planes being used
    *((int16_t *) &(bmp_info_header[14])) = (int16_t) 24;  // number of bits per pixel

    if (fwrite(bmp_file_header, 1, sizeof(bmp_file_header), fp) != sizeof(bmp_file_header)) {
        goto out;
    }
    if (fwrite(bmp_info_header, 1, sizeof(bmp_info_header), fp) != sizeof(bmp_info_header)) {
        goto out;
    }

    const int line_size = im->width * 3 + (4 - (im->width * 3) % 4) % 4;
    bgr_row = (uint8_t *) malloc(line_size);
    memset(bgr_row, 0, line_size);
    int y;
    for (y = 0; y < im->height; y++) {
        uint8_t* rgb_row = im->data[im->height - y - 1];
        int x;
        for (x = 0; x < im->width; x++) {
            bgr_row[x * 3 + 2] = rgb_row[x * 3 + 0];
            bgr_row[x * 3 + 1] = rgb_row[x * 3 + 1];
            bgr_row[x * 3 + 0] = rgb_row[x * 3 + 2];
        }
        if (fwrite(bgr_row, 1, line_size, fp) != line_size) {
            goto out;
        }
    }
    r = 0;
out:
    if (fp != NULL) {
        fclose(fp);
    }
    if (bgr_row != NULL) {
        free(bgr_row);
    }
    return r;
}
