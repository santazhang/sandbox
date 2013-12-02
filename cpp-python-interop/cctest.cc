#include <stdio.h>

#include "tinymath.h"

using namespace tinymath;

int main() {
    TinyMath tm;
    printf("gcd(5, 3)=%d\n", tm.gcd(5, 3));
    return 0;
}
