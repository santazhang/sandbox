#pragma once

#include <inttypes.h>

namespace find_trees {

int debug_write_ppm(const char* fn, int width, int height, uint8_t* red, uint8_t* green, uint8_t* blue);

int debug_write_pgm(const char* fn, int width, int height, uint8_t* gray);

}  // namespace find_trees
