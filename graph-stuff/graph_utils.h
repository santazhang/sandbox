#pragma once

#include <utility>
#include <vector>
#include <algorithm>
#include <functional>
#include <string>

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <sys/time.h>

#include "hyperloglog/hyperloglog.hpp"

// http://stackoverflow.com/a/31798726/1035246
// Workaround missing "is_trivially_copyable" in g++ < 5.0
#if __GNUG__ && __GNUC__ < 5
#ifndef __APPLE__
#define IS_TRIVIALLY_COPYABLE(T) __has_trivial_copy(T)
namespace std {

template <typename T>
struct is_trivially_copy_constructible {
    static const bool value = __has_trivial_copy(T);
};

}  // namespace std
#endif  // #ifndef __APPLE__
#endif  // __GNUG__ && __GNUC__ < 5

#include "sparsehash/sparse_hash_map"
#include "sparsehash/sparse_hash_set"

const int MAX_CLUSTERS = 1000 * 1000;

using node_t = int32_t;
using edge_t = std::pair<int32_t, int32_t>;

class Timer {
    struct timeval begin_;
    struct timeval end_;

public:
    Timer() { reset(); }
    inline void start() {
        reset();
        gettimeofday(&begin_, nullptr);
    }
    inline void stop() {
        gettimeofday(&end_, nullptr);
    }
    inline void reset() {
        begin_.tv_sec = 0;
        begin_.tv_usec = 0;
        end_.tv_sec = 0;
        end_.tv_usec = 0;
    }
    double elapsed() const {
        if (end_.tv_sec == 0 && end_.tv_usec == 0) {
            // not stopped yet
            struct timeval now;
            gettimeofday(&now, nullptr);
            return now.tv_sec - begin_.tv_sec +
                (now.tv_usec - begin_.tv_usec) / 1000000.0;
        }
        return end_.tv_sec - begin_.tv_sec +
            (end_.tv_usec - begin_.tv_usec) / 1000000.0;
    }
};

class Progress {
public:
    Progress() {
        begin("Progress", 100);
    }

    void begin(const char* descr, size_t total_work) {
        descr_ = descr;
        total_work_ = total_work;
        if (total_work_ == 0) {
            total_work_ = 1;
        }
        done_work_ = 0;
        last_percentage_ = -1;
        finished_ = false;
        timer_.start();
    }

    inline void step(int incr = 1) {
        done_work_ += incr;
        int percentage = 100L * done_work_ / total_work_;
        if (percentage >= 100) {
            percentage = 100;
            if (!finished_) {
                printf("%s: %3d%%\n", descr_, percentage);
            }
            finished_ = true;
        } else {
            if (percentage != last_percentage_ && timer_.elapsed() >= 1.0) {
                printf("%s: %3d%%\n", descr_, percentage);
                timer_.start();
            }
        }
        last_percentage_ = percentage;
    }

private:
    const char* descr_;
    size_t total_work_;
    size_t done_work_;
    int last_percentage_;
    bool finished_;
    Timer timer_;
};

class ScopedCleaner {
public:
    explicit ScopedCleaner(const std::function<void()>& cleaner)
        : cleaner_(cleaner) { }
    ~ScopedCleaner() {
        cleaner_();
    }
private:
    std::function<void()> cleaner_;
};

class ReportFunctionTimingAfterReturn {
public:
    ReportFunctionTimingAfterReturn(const char* func): func_(func) {
        printf("  *** Start function `%s`.\n", func_);
        t_.start();
    }
    ~ReportFunctionTimingAfterReturn() {
        t_.stop();
        printf("  *** Function `%s` took %.6lf seconds.\n",
               func_, t_.elapsed());
    }

private:
    Timer t_;
    const char* func_;
};
#define REPORT_FUNCTION_TIMING_AFTER_RETURN \
    ReportFunctionTimingAfterReturn __Do_NoT_uSe__RfTaR__(__PRETTY_FUNCTION__)

static int32_t my_rand() {
    int32_t v = rand();
    v <<= 16;
    v += rand();
    return v < 0 ? -v : v;
}

static void my_srand() {
    int seed = ::getpid();
    uint32_t hi, lo;
    __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
    seed ^= lo;
    FILE* fp = fopen("/dev/urandom", "r");
    if (fp) {
        int v;
        if (fread(&v, sizeof(v), 1, fp) == 1) {
            seed ^= v;
        }
        ::fclose(fp);
    }
    ::srand(seed);
    rand();
    rand();
    rand();
    rand();
    rand();
}

static int32_t my_srand_then_rand() {
    my_srand();
    return my_rand();
}


namespace std {

template <>
struct hash<edge_t> {
    size_t operator() (edge_t e) const {
        static const size_t mix = hash<int32_t>()(my_srand_then_rand());
        return (h_(e.first) * 0x9e3779b97f4a7c15LLU) ^ h_(e.second) ^ mix;
    }

private:
    hash<node_t> h_;
};

}  // namespace std


static int estimate_node_count(
        const google::sparse_hash_map<edge_t, int32_t>& clustering) {

    Timer timer;
    timer.start();
    Progress progress;

    hll::HyperLogLogHIP h(16);
    progress.begin("Estimate node count", clustering.size());
    for (const auto& it : clustering) {
        progress.step();
        h.add(reinterpret_cast<const char*>(&it.first.first), sizeof(node_t));
        h.add(reinterpret_cast<const char*>(&it.first.second), sizeof(node_t));
    }

    timer.stop();
    double est = h.estimate();
    printf("Estimated node count ~= %lf in %.6lf seconds.\n",
           est, timer.elapsed());

    return est;
}

static int estimate_cluster_count(
        const google::sparse_hash_map<edge_t, int32_t>& clustering) {

    Timer timer;
    timer.start();
    Progress progress;

    hll::HyperLogLogHIP h(16);
    progress.begin("Estimate cluster count", clustering.size());
    for (const auto& it : clustering) {
        progress.step();
        h.add(reinterpret_cast<const char*>(&it.second), sizeof(int32_t));
    }

    timer.stop();
    double est = h.estimate();
    printf("Estimated cluster count ~= %lf in %.6lf seconds.\n",
           est, timer.elapsed());

    return est;
}

static int estimate_src_cluster_pair_count(
        const google::sparse_hash_map<edge_t, int32_t>& clustering) {

    Timer timer;
    timer.start();
    Progress progress;

    hll::HyperLogLogHIP h(16);
    progress.begin("Estimate src-cluster pair count", clustering.size());
    std::pair<node_t, int32_t> data;
    for (const auto& it : clustering) {
        progress.step();
        data.first = it.first.first;
        data.second = it.second;
        h.add(reinterpret_cast<const char*>(&data), sizeof(data));
    }

    timer.stop();
    double est = h.estimate();
    printf("Estimated src-cluster pair count ~= %lf in %.6lf seconds.\n",
           est, timer.elapsed());

    return est;
}


static void eval_edge_clustering(
        const google::sparse_hash_map<edge_t, int32_t>& clustering) {

    Timer timer;
    timer.start();
    Progress progress;

    int est_nodes = estimate_node_count(clustering);
    int est_src_cluster = estimate_src_cluster_pair_count(clustering);

    {
        google::sparse_hash_set<std::pair<node_t, int32_t>> src_to_cluster;
        src_to_cluster.resize(est_src_cluster);
        google::sparse_hash_map<node_t, int32_t> src_to_cluster_count;
        src_to_cluster_count.resize(est_nodes);
        progress.begin("Collect src_to_cluster_count", clustering.size());
        for (const auto& it : clustering) {
            progress.step();
            node_t src = it.first.first;
            int32_t cluster = it.second;
            std::pair<node_t, int32_t> src_cluster { src, cluster };
            if (src_to_cluster.find(src_cluster) == src_to_cluster.end()) {
                src_to_cluster.insert(src_cluster);
                src_to_cluster_count[src]++;
            }
        }
        google::sparse_hash_map<int32_t, int32_t> fanout_by_src_distribution;
        progress.begin("Collect fanout_by_src_distribution",
                       src_to_cluster_count.size());
        for (const auto& it : src_to_cluster_count) {
            progress.step();
            int32_t fanout_count = it.second;
            fanout_by_src_distribution[fanout_count]++;
        }
        printf("Fanout (by source node):\n");
        std::vector<std::pair<int32_t, int32_t>> sorted_distribution;
        for (const auto& it : fanout_by_src_distribution) {
            sorted_distribution.push_back(it);
        }
        std::sort(sorted_distribution.begin(), sorted_distribution.end());
        for (const auto& it : sorted_distribution) {
            int32_t fanout_count = it.first;
            int32_t src_count = it.second;
            printf("  %d nodes have outgoing edges on %d clusters.\n",
                   src_count, fanout_count);
        }
    }

    timer.stop();
    printf("Evaluated edge clustering in %.6lf seconds.\n", timer.elapsed());
}
