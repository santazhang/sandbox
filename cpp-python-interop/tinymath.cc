#include "tinymath.h"

namespace tinymath {

int TinyMath::gcd(int a, int b) const {
    int r = a % b;
    while (r != 0) {
        a = b;
        b = r;
        r = a % b;
    }
    return b;
}

} // tinymath

