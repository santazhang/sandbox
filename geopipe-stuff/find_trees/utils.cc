#include "./utils.h"
#include <math.h>
#include <stdio.h>

namespace find_trees {

void Timer::pause() {
    if (paused_) {
        printf("Timer::pause(): timer already paused!\n");
    } else {
        base_elapsed_ = elapsed();
        clear_begin_end_time();
        paused_ = true;
    }
}

void Timer::unpause() {
    if (paused_) {
        gettimeofday(&begin_, nullptr);
        paused_ = false;
    } else {
        printf("Timer::unpause(): timer not paused!\n");
    }
}

double Timer::elapsed() const {
    if (end_.tv_sec == 0 && end_.tv_usec == 0) {
        // not stopped yet
        struct timeval now;
        gettimeofday(&now, nullptr);
        return base_elapsed_ + now.tv_sec - begin_.tv_sec +
               (now.tv_usec - begin_.tv_usec) / 1000000.0;
    }
    return base_elapsed_ + end_.tv_sec - begin_.tv_sec +
           (end_.tv_usec - begin_.tv_usec) / 1000000.0;
}

void Progress::print_progress(int percentage) {
    if (percentage >= 100) {
        printf("  ... %s: 100%% (%ld/%ld)\n", descr_.c_str(), (long)done_work_, (long)total_work_);
    } else {
        char buf[5];
        ::snprintf(buf, sizeof(buf), "%3d%%", percentage);
        printf("  ... %s: %s (%ld/%ld)\n", descr_.c_str(), buf, (long)done_work_, (long)total_work_);
        timer_.start();  // restart timer
    }
}


double weighted_mean(int n_samples, double* weights, double* samples) {
    double s = 0, sw = 0;
    for (int i = 0; i < n_samples; i++) {
        s += weights[i] * samples[i];
        sw += weights[i];
    }
    return s / sw;
}

double weighted_variance(int n_samples, double* weights, double* samples) {
    double m = weighted_mean(n_samples, weights, samples);
    double sw = 0, sw2 = 0, s = 0;
    for (int i = 0; i < n_samples; i++) {
        sw += weights[i];
        sw2 += weights[i] * weights[i];
        s += weights[i] * (samples[i] - m) * (samples[i] - m);
    }
    return s / (sw - sw2/sw);
}

double weighted_stddev(int n_samples, double* weights, double* samples) {
    return sqrt(weighted_variance(n_samples, weights, samples));
}

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
