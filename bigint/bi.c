#include <string.h>

#ifdef __APPLE__
#include <stdlib.h>
#else
#include <malloc.h>
#endif

#include "bi.h"

typedef unsigned int word;

struct __bi {
    int len;    // valid data size
    int maxlen; // memory size
    int sign;   // -1, 0, 1
    word* data; // data[0] is lowest bits
};

bi bi_new() {
    bi v = (bi) malloc(sizeof(struct __bi));
    v->len = 1;
    v->maxlen = 1;
    v->sign = 0;
    v->data = (word *) malloc(sizeof(word) * v->maxlen);
    v->data[0] = 0;
    return v;
}

void bi_free(bi v) {
    free(v->data);
    free(v);
}
