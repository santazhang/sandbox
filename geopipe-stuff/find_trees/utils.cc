#include "./utils.h"

namespace find_trees {

void neighbor_grids_3x3(int grid_rows, int grid_cols, int current_grid_id,
                        int* neighbor_grid_ids, int* p_neighbor_grid_count) {

    int& neighbor_grid_count = *p_neighbor_grid_count;
    int ga = current_grid_id;
    int ga_row = ga / grid_cols;
    int ga_col = ga % grid_cols;

    neighbor_grid_ids[0] = ga;
    neighbor_grid_count = 1;

    // 3 grids above ga
    if (ga_row > 0) {
        if (ga_col > 0) {
            neighbor_grid_ids[neighbor_grid_count] = ga - grid_cols - 1;
            neighbor_grid_count++;
        }
        neighbor_grid_ids[neighbor_grid_count] = ga - grid_cols;
        neighbor_grid_count++;
        if (ga_col != grid_cols - 1) {
            neighbor_grid_ids[neighbor_grid_count] = ga - grid_cols + 1;
            neighbor_grid_count++;
        }
    }

    // left and right
    if (ga_col > 0) {
        neighbor_grid_ids[neighbor_grid_count] = ga - 1;
        neighbor_grid_count++;
    }
    if (ga_col != grid_cols - 1) {
        neighbor_grid_ids[neighbor_grid_count] = ga + 1;
        neighbor_grid_count++;
    }

    // 3 grids below ga
    if (ga_row < grid_rows - 1) {
        if (ga_col > 0) {
            neighbor_grid_ids[neighbor_grid_count] = ga + grid_cols - 1;
            neighbor_grid_count++;
        }
        neighbor_grid_ids[neighbor_grid_count] = ga + grid_cols;
        neighbor_grid_count++;
        if (ga_col != grid_cols - 1) {
            neighbor_grid_ids[neighbor_grid_count] = ga + grid_cols + 1;
            neighbor_grid_count++;
        }
    }
}

}  // namespace find_trees
