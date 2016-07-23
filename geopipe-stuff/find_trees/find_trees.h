#pragma once

#include <inttypes.h>
#include <vector>

#include "las_stats.h"

namespace find_trees {

struct params_t {
    int img_width;
    int img_height;

    // no ownership; contiguous bytes
    const uint8_t* channel_red;
    const uint8_t* channel_green;
    const uint8_t* channel_blue;
    const uint8_t* channel_ir = nullptr;
    
    // las info, also no ownership
    const point3d_t* points = nullptr;
    int n_points = -1;
    double points_min_x = 0;
    double points_max_x = 0;
    double points_min_y = 0;
    double points_max_y = 0;
    double points_resolution = 0;  // 2016.07: only support 0.15 (for 0x1500 images)

    // based on 0x1500 orthoimages, max tree radius = 10 meters
    int max_tree_radius = 67;

    void reset() {
        img_width = -1;
        img_height = -1;
        channel_red = nullptr;
        channel_green = nullptr;
        channel_blue = nullptr;
        channel_ir = nullptr;
        points = nullptr;
        n_points = -1;
        points_min_x = 0;
        points_max_x = 0;
        points_min_y = 0;
        points_max_y = 0;
        points_resolution = 0;
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
    int tile_rows = -1;
    int tile_cols = -1;
    int tile_step_x = -1;
    int tile_step_y = -1;

    // NOTE: if there's a band of pixels on the rigth side (or bottom), that
    // is > 0px thick, but less than tile_size thick, it is still treated as a
    // band of valid tile. And it also extends backwards (to left, or to above),
    // extending width to tile_size
    std::vector<int> tree_tiles;
};

int find(const params_t& params, result_t* result);

}  // namespace find_trees
