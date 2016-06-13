#pragma once

#include <inttypes.h>
#include <vector>

namespace find_trees {

struct params_t {
    int img_width;
    int img_height;

    // no ownership; contiguous bytes
    const uint8_t* channel_red;
    const uint8_t* channel_green;
    const uint8_t* channel_blue;

    // based on 0x1500 orthoimages, max tree radius = 10 meters
    int max_tree_radius = 67;

    void reset() {
        img_width = -1;
        img_height = -1;
        channel_red = nullptr;
        channel_green = nullptr;
        channel_blue = nullptr;
    }
    params_t() {
        reset();
    }
};

struct tree_info_t {
    double x_pixels = -1.0;  // range: [0, img.width)
    double y_pixels = -1.0;  // range: [0, img.height)
    double radius_pixels = -1.0;  // range: [0, max_tree_radius]

    bool operator ==(const tree_info_t& o) const {
        return x_pixels == o.x_pixels &&
               y_pixels == o.y_pixels &&
               radius_pixels == o.radius_pixels;
    }
};

struct result_t {
    std::vector<tree_info_t> trees;
    int tile_size = -1;
    std::vector<int> tree_tiles;
};

int find(const params_t& params, result_t* result);

}  // namespace find_trees
