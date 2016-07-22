#pragma once

#include <string>
#include <sys/time.h>

namespace find_trees {

class Timer {
    struct timeval begin_;
    struct timeval end_;
    bool paused_;
    double base_elapsed_;

    inline void clear_begin_end_time() {
        begin_.tv_sec = 0;
        begin_.tv_usec = 0;
        end_.tv_sec = 0;
        end_.tv_usec = 0;
    }

public:
    Timer() { reset(); }
    inline void start() {
        reset();
        gettimeofday(&begin_, nullptr);
    }
    inline void stop() {
        gettimeofday(&end_, nullptr);
    }
    void pause();
    void unpause();
    inline void reset() {
        clear_begin_end_time();
        base_elapsed_ = 0.0;
        paused_ = false;
    }
    double elapsed() const;
};

// Track progress
class Progress {
public:
    Progress() {
        begin("Progress", 100);
    }

    void begin(const std::string& descr, size_t total_work) {
        descr_ = descr;
        total_work_ = total_work;
        done_work_ = 0;
        last_percentage_ = -1;
        finished_ = false;
        timer_.start();
    }

    inline void step(size_t incr = 1) {
        if (finished_) {
            return;
        }
        done_work_ += incr;
        int percentage = 100.0 * done_work_ / total_work_;
        if (percentage >= 100) {
            percentage = 100;
            done_work_ = total_work_;
            if (!finished_) {
                print_progress(percentage);
            }
            finished_ = true;
        } else {
            const double report_interval_sec = 1.0;  // report frequency
            if (percentage != last_percentage_ &&
                timer_.elapsed() >= report_interval_sec
               ) {
                print_progress(percentage);
            }
        }
        last_percentage_ = percentage;
    }

private:
    void print_progress(int percentage);

    std::string descr_;
    size_t total_work_;
    size_t done_work_;
    int last_percentage_;
    bool finished_;
    Timer timer_;
};

class ProgressTicker {
public:
    explicit ProgressTicker(Progress* progress, size_t incr = 1)
         : progress_(progress), incr_(incr) { }

    ~ProgressTicker() {
        progress_->step(incr_);
    }

private:
    Progress* progress_;
    size_t incr_;
};


double weighted_mean(int n_samples, double* weights, double* samples);
double weighted_variance(int n_samples, double* weights, double* samples);
double weighted_stddev(int n_samples, double* weights, double* samples);

void neighbor_grids_3x3(int grid_rows, int grid_cols, int current_grid_id,
                        int* neighbor_grid_ids, int* neighbor_grid_count);

}  // namespace find_trees
