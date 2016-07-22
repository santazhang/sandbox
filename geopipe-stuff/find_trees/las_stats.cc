#include "las_stats.h"
#include "./utils.h"

#include <math.h>
#include <glog/logging.h>

#include <vector>

using std::vector;

namespace find_trees {

void points_stats(point3d_t* points, int n_points,
                  double min_x, double max_x, double min_y, double max_y,
                  double resolution,
                  double* output_stddev_z) {

/*
    How it works:
    * Cut the input tile into 1meter x 1meter grids.
    * Do a sliding window of 5x5 grids, calculate a gaussian stddev for the center box (center pixel).
      (https://en.wikipedia.org/wiki/Weighted_arithmetic_mean#Weighted_sample_variance)
    * Bilinear interpolate to all pixels. (take special care of borders)
*/

    const int img_width = ceilf((max_x - min_x) / resolution);
    const int img_height = ceilf((max_y - min_y) / resolution);
    const int img_pixels = img_width * img_height;
    const int grid_size = ceilf(1.0 / resolution);
    const int grid_rows = (img_height + grid_size - 1) / grid_size;
    const int grid_cols = (img_width + grid_size - 1) / grid_size;
    const int grid_count = grid_rows * grid_cols;
    LOG(INFO) << "grid size: " << grid_size << " x " << grid_size;
    LOG(INFO) << "cutting output into " << grid_cols << "col x " << grid_rows << "rows";

    vector<vector<point3d_t>> grid;
    grid.resize(grid_count);

    // helper function
    auto get_grid_id = [grid_size, grid_cols] (int x, int y) -> int {
        return (y / grid_size) * grid_cols + (x / grid_size);
    };

    for (int i = 0; i < n_points; i++) {
        point3d_t pt = points[i];
        int x_pixels = quantize(pt.x, min_x, max_x, img_width);
        int y_pixels = img_height - 1 - quantize(pt.y, min_y, max_y, img_height);
        int grid_id = get_grid_id(x_pixels, y_pixels);
        grid[grid_id].push_back(pt);
    }

    Progress progress;
    progress.begin("calculate z hint", grid_rows * grid_cols);
    for (int center_grid_y = 0; center_grid_y < grid_rows; center_grid_y++) {
        for (int center_grid_x = 0; center_grid_x < grid_cols; center_grid_x++) {
            ProgressTicker ticker(&progress);
            int center_grid_id = center_grid_y * grid_cols + center_grid_x;

            // http://stats.stackexchange.com/a/171619
            double sum_w = 0;  // sum(w_i)
            double sum_w2 = 0;  // sum(w_i^2)
            double sum_wz = 0;  // sum(w_i * z_i)
            std::vector<double> z_samples;
            std::vector<double> z_weights;

            // go over 5x5 grid
            for (int roi_grid_y = center_grid_y - 2; roi_grid_y <= center_grid_y + 2; roi_grid_y++) {
                if (roi_grid_y < 0 || roi_grid_y >= grid_rows) {
                    continue;
                }
                for (int roi_grid_x = center_grid_x - 2; roi_grid_x <= center_grid_x + 2; roi_grid_x++) {
                    if (roi_grid_x < 0 || roi_grid_x >= grid_cols) {
                        continue;
                    }
                    const int grid_dist_x = abs(roi_grid_x - center_grid_x);
                    const int grid_dist_y = abs(roi_grid_y - center_grid_y);
                    // see https://en.wikipedia.org/wiki/Gaussian_blur
                    const double gaussian_weight = ::exp(-0.25 * (grid_dist_x*grid_dist_x + grid_dist_y*grid_dist_y));
                    // printf("%0.6lf ", gaussian_weight);
                    int roi_grid_id = roi_grid_y * grid_cols + roi_grid_x;

                    for (const auto& pt : grid[roi_grid_id]) {
                        // process one sample point, weight=gaussian_weight, z=pt.z
                        sum_w += gaussian_weight;
                        sum_wz += gaussian_weight * pt.z;
                        z_samples.push_back(pt.z);
                        z_weights.push_back(gaussian_weight);
                    }
                }
                // printf("\n");
            }
            // printf("\n");

            // http://stats.stackexchange.com/a/171619
            double weighted_mean_z = sum_wz/sum_w;
            // printf("weighted_mean_z=%lf\n", weighted_mean_z);
            double weighted_var_z = 0;
            for (size_t i = 0; i < z_samples.size(); i++) {
                double diff_z = z_samples[i] - weighted_mean_z;
                weighted_var_z += z_weights[i] * diff_z*diff_z;
            }
            weighted_var_z /= sum_w - sum_w2/sum_w;
            double weighted_stddev_z = ::sqrt(weighted_var_z);
            //printf("weighted_stddev_z=%lf\n", weighted_stddev_z);
            int y_pixel = center_grid_y * grid_size + grid_size / 2;
            int x_pixel = center_grid_x * grid_size + grid_size / 2;
            if (y_pixel < img_height && x_pixel < img_width) {
                output_stddev_z[y_pixel*img_width + x_pixel] = weighted_stddev_z;
            }

            // debug code
            for (int y = center_grid_y * grid_size; y < (center_grid_y + 1) * grid_size && y < img_height; y++) {
                for (int x = center_grid_x * grid_size; x < (center_grid_x + 1) * grid_size && x < img_width; x++) {
                    output_stddev_z[y * img_width + x] = weighted_stddev_z;
                }
            }
        }
    }

    // bilinear interpolation (with special handling on border)
    // https://en.wikipedia.org/wiki/Bilinear_interpolation
    for (int grid_y = 0; grid_y < grid_rows; grid_y++) {
        for (int grid_x = 0; grid_x < grid_cols; grid_x++) {
            int x0 = grid_x*grid_size + grid_size/2;
            int x1 = (grid_x+1)*grid_size + grid_size/2;
            int y0 = grid_y*grid_size + grid_size/2;
            int y1 = (grid_y+1)*grid_size + grid_size/2;

            // special border handling -> back off 1 grid
            if (grid_x == grid_cols) {
                x0 -= grid_size;
                x1 -= grid_size;
            }
            if (grid_y == grid_rows) {
                y0 -= grid_size;
                y1 -= grid_size;
            }

            double z00 = 0;
            if (y0 < img_height && x0 < img_width) {
                z00 = output_stddev_z[y0*img_width + x0];
            }
            double z01 = 0;
            if (y0 < img_height && x1 < img_width) {
                z01 = output_stddev_z[y0*img_width + x1];
            }
            double z10 = 0;
            if (y1 < img_height && x0 < img_width) {
                z10 = output_stddev_z[y1*img_width + x0];
            }
            double z11 = 0;
            if (y1 < img_height && x1 < img_width) {
                z11 = output_stddev_z[y1*img_width + x1];
            }

            for (int yy = y0; yy < y1 && yy < img_height; yy++) {
                for (int xx = x0; xx < x1 && xx < img_width; xx++) {
                    double rx = double(xx - x0) / grid_size;  // 0->1
                    double ry = double(yy - y0) / grid_size;  // 0->1
                    output_stddev_z[yy*img_width + xx] = (1 - rx) * (1 - ry) * z00 +
                                                       rx * (1 - ry) * z01 +
                                                       (1-rx) * ry * z10 +
                                                       rx*ry * z11;
                }
            }
        }
    }
}

}  // namespace find_trees
