#include <string.h>

#include "im.h"

image im_new(int width, int height) {
    image im = (image) malloc(sizeof(struct __image));
    im->width = width;
    im->height = height;
    im->data = (uint8_t **) malloc(sizeof(uint8_t *) * im->height);
    int i, row_bytes = im->width * 3;
    for (i = 0; i < im->height; i++) {
        im->data[i] = (uint8_t *) malloc(row_bytes);
        memset(im->data[i], 0, row_bytes);
    }
    return im;
}

image im_dup(image im) {
    image dup = (image) malloc(sizeof(struct __image));
    dup->width = im->width;
    dup->height = im->height;
    dup->data = (uint8_t **) malloc(sizeof(uint8_t *) * dup->height);
    int i, row_bytes = dup->width * 3;
    for (i = 0; i < dup->height; i++) {
        dup->data[i] = (uint8_t *) malloc(row_bytes);
        memcpy(dup->data[i], im->data[i], row_bytes);
    }
    return dup;
}

void im_free(image im) {
    if (im == NULL) {
        return;
    }
    int i;
    for (i = 0; i < im->height; i++) {
        free(im->data[i]);
    }
    free(im->data);
}
