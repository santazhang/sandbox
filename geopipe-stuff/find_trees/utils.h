#pragma once

namespace find_trees {

double weighted_mean(int n_samples, double* weights, double* samples);
double weighted_variance(int n_samples, double* weights, double* samples);
double weighted_stddev(int n_samples, double* weights, double* samples);

void neighbor_grids_3x3(int grid_rows, int grid_cols, int current_grid_id,
                        int* neighbor_grid_ids, int* neighbor_grid_count);

}  // namespace find_trees
