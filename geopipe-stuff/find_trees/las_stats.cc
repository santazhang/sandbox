#include "las_stats.h"
#include "./utils.h"

#include <math.h>
#include <glog/logging.h>

#include <vector>

using std::vector;

namespace find_trees {

void points_stats(point3d_t* points, int n_points,
                  double min_x, double max_x, double min_y, double max_y,
                  double resolution, double radius_in_meters,
                  double* output_weight,
                  double* output_stddev_z) {

    const int img_width = ceilf((max_x - min_x) / resolution);
    const int img_height = ceilf((max_y - min_y) / resolution);
    const int img_pixels = img_width * img_height;
    const int grid_size = 2 * (radius_in_meters / resolution + 1);
    const int grid_rows = (img_height + grid_size - 1) / grid_size;
    const int grid_cols = (img_width + grid_size - 1) / grid_size;
    const int grid_count = grid_rows * grid_cols;
    LOG(INFO) << "grid size: " << grid_size << " x " << grid_size;
    LOG(INFO) << "cutting output into " << grid_cols << "col x " << grid_rows << "rows";

    // helper function
    auto get_grid_id = [grid_size, grid_cols] (int x, int y) -> int {
        return (y / grid_size) * grid_cols + (x / grid_size);
    };

    vector<vector<point3d_t>> grid;
    grid.resize(grid_count);

    for (int i = 0; i < n_points; i++) {
        point3d_t pt = points[i];
        int x_pixels = quantize(pt.x, min_x, max_x, img_width);
        int y_pixels = img_height - 1 - quantize(pt.y, min_y, max_y, img_height);
        int grid_id = get_grid_id(x_pixels, y_pixels);
        grid[grid_id].push_back(pt);
    }

    const double alpha = 100.0 / (grid_size * grid_size);
    const int dist_cut2 = grid_size * grid_size;
    int last_pct = -1;
    for (int grid_i = 0; grid_i < grid_count; grid_i++) {
        {
            int pct = grid_i * 100 / grid_count;
            if (pct != last_pct) {
                last_pct = pct;
                LOG(INFO) << "Processing done " << pct << "%";
            }
            if (pct >= 3) {
                break;
            }
        }

        int neighbor_grid_ids[9];
        int neighbor_grid_count;

        neighbor_grids_3x3(grid_rows, grid_cols, grid_i,
                           neighbor_grid_ids, &neighbor_grid_count);

        int grid_r = grid_i / grid_cols;
        int grid_c = grid_i % grid_cols;

        for (const auto& pt : grid[grid_i]) {
            int x1_pixels = quantize(pt.x, min_x, max_x, img_width);
            int y1_pixels = img_height - 1 - quantize(pt.y, min_y, max_y, img_height);

            vector<double> weights;
            vector<double> z_samples;
            for (int neighbor_i = 0; neighbor_i < neighbor_grid_count; neighbor_i++) {
                for (const auto& pt2 : grid[neighbor_grid_ids[neighbor_i]]) {
                    int x2_pixels = quantize(pt2.x, min_x, max_x, img_width);
                    int y2_pixels = img_height - 1 - quantize(pt2.y, min_y, max_y, img_height);
                    int dx = x1_pixels - x2_pixels;
                    int dy = y1_pixels - y2_pixels;
                    int d2 = dx*dx + dy*dy;

                    if (d2 < dist_cut2) {
                        double weight = 1.0 / (1.0 + d2 * alpha);
                        weights.push_back(weight);
                        z_samples.push_back(pt2.z);
                    }
                }
            }

            double w_stddev = weighted_stddev(weights.size(), &weights[0], &z_samples[0]);
            double w = w_stddev * 25;
            if (w > 255) {
                w = 255;
            }
            output_weight[y1_pixels * img_width + x1_pixels] = w;
        }

        // for (int y = grid_r * grid_size, y_off = y * img_width;
        //      y < (grid_r + 1) * grid_size && y < img_height;
        //      y++, y_off += img_width)
        // {
        //     for (int x = grid_c * grid_size; x < (grid_c + 1) * grid_size && x < img_width; x++) {
        //         output_weight[y_off + x] = 128;
        //     }
        // }
    }

}

}  // namespace find_trees
