#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <inttypes.h>

#include "miller-rabin/sprp32.h"
#include "miller-rabin/sprp64.h"

#define verify(exp) \
    do { \
        if (!(exp)) { \
            ::printf("  *** verification failed: %s (%s:%d)\n", #exp, __FILE__, __LINE__); \
            ::abort(); \
        } \
    } while(0)

static bool is_prime(int64_t n) {
    verify(n > 0);

    if (n == 1) {
        return false;
    } else if (n == 2) {
        return true;
    } else if (n % 2 == 0) {
        return false;
    } else if (n == 3) {
        return true;
    } else if (n % 3 == 0) {
        return false;
    } else if (n == 5) {
        return true;
    } else if (n % 5 == 0) {
        return false;
    } else if (n == 7) {
        return true;
    } else if (n % 7 == 0) {
        return false;
    } else {
        // https://miller-rabin.appspot.com/
        static const uint64_t bases[7] = {2, 325, 9375, 28178, 450775, 9780504, 1795265022};
        return efficient_mr64(bases, 7, n);
    }
}
