#pragma once

#include "im.h"

// return NULL on failure; only 8 bit RGB images supported
image impng_load(const char* path);

// return -1 on failure, 0 on success
int impng_save(image im, const char* path);
