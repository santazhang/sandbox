#include <stdio.h>

#include "bi.h"

int main(int argc, char* argv[]) {
    printf("This shall be done!\n");
    bi v1 = bi_new();
    bi_free(v1);
    return 0;
}
