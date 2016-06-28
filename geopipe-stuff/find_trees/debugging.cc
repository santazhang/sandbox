#include "debugging.h"

#include <stdio.h>

namespace find_trees {

int debug_write_ppm(const char* fn, int width, int height, uint8_t* red, uint8_t* green, uint8_t* blue) {
    FILE* fp = fopen(fn, "wb");
    if (fp == NULL) {
        return -1;
    }
    fprintf(fp, "P6\n%d %d\n255\n", width, height);
    for (int y = 0, y_off = 0; y < height; y++, y_off += width) {
        for (int x = 0; x < width; x++) {
            fwrite(&red[y_off + x], sizeof(uint8_t), 1, fp);
            fwrite(&green[y_off + x], sizeof(uint8_t), 1, fp);
            fwrite(&blue[y_off + x], sizeof(uint8_t), 1, fp);
        }
    }
    fclose(fp);
    return 0;
}

int debug_write_pgm(const char* fn, int width, int height, uint8_t* gray) {
    FILE* fp = fopen(fn, "wb");
    if (fp == NULL) {
        return -1;
    }
    fprintf(fp, "P5\n%d %d\n255\n", width, height);
    fwrite(gray, sizeof(uint8_t), width * height, fp);
    fclose(fp);
    return 0;
}

}  // namespace find_trees
