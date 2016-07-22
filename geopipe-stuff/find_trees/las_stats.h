#pragma once

#include <math.h>

namespace find_trees {

struct point3d_t {
    double x;
    double y;
    double z;
};

inline int quantize(double value, double lb, double ub, int levels) {
    return nearbyint((levels - 1) * (value - lb) / (ub - lb));
}

void points_stats(point3d_t* points, int n_points,
                  double min_x, double max_x, double min_y, double max_y,  // determines bounding box
                  double resolution,
                  double* output_stddev_z);

}  // namespace find_trees
