#pragma once

#include <fstream>
#include <utility>
#include <vector>
#include <algorithm>
#include <functional>
#include <string>

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <inttypes.h>
#include <sys/time.h>
#include <unistd.h>

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

const node_t BAD_NODE = -1;

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


static int estimate_edges_in_txt_file(const char* fpath) {
    int est_edges = 0;
    std::ifstream fin(fpath);
    fin.seekg(0, std::ios_base::end);
    size_t fsize = fin.tellg();
    printf("File %s size: %ld\n", fpath, fsize);
    if (fsize > 1024 * 1024) {
        const int n_sample = 100;
        double sample_len_sum = 0.0;
        std::string line;
        for (size_t i = 0; i < n_sample; i++) {
            fin.seekg(i * fsize / n_sample, std::ios_base::beg);
            getline(fin, line);  // discard first (partially read) line
            getline(fin, line);
            sample_len_sum += line.size() + 1;  // +1 for \n
        }
        if (sample_len_sum / n_sample > 0) {
            est_edges = fsize / (sample_len_sum / n_sample);
            printf("Estimate file %s has ~= %d edges.\n", fpath, est_edges);
        }
    }
    return est_edges;
}

static size_t round_down(size_t val, size_t align) {
    if (align <= 1) {
        return val;
    }
    return (val / align) * align;
}

static int estimate_edges_in_adj_file(const std::string& fpath) {
    int est_edges = 0;
    FILE* fp = fopen(fpath.c_str(), "rb");
    fseek(fp, 0, SEEK_END);
    size_t fsize = ftell(fp);
    printf("File %s size: %ld\n", fpath.c_str(), fsize);
    if (fsize > 1024 * 1024) {
        const int n_sample = 100;
        double sample_edge_bytes = 0.0;
        double sample_all_bytes = 0.0;
        for (size_t i = 0; i < n_sample; i++) {
            fseek(fp, round_down(i * fsize / n_sample, sizeof(node_t)), SEEK_SET);

            // discard first (partially read) adjlist
            node_t u = BAD_NODE;
            do {
                fread(&u, 1, sizeof(node_t), fp);
            } while (u != BAD_NODE);

            // second adjlist can be used for analysis
            u = BAD_NODE;
            int count = 0;
            do {
                fread(&u, 1, sizeof(node_t), fp);
                count++;
            } while (u != BAD_NODE);

            sample_all_bytes += count * sizeof(node_t);
            // do not include <src> and <separator>
            sample_edge_bytes += (count - 2) * sizeof(node_t);
        }
        est_edges = (fsize * (sample_edge_bytes / sample_all_bytes)) / sizeof(node_t);
        printf("Estimate file %s has ~= %d edges.\n", fpath.c_str(), est_edges);
    }
    return est_edges;
}

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

void load_adjfile(const char* adjfn, std::function<void(edge_t)>&& cb) {
    Timer timer;
    timer.start();

    FILE* fp = fopen(adjfn, "rb");
    if (fp == nullptr) {
        printf("  *** Failed to open for read: %s\n", adjfn);
        exit(1);
    }

    fseek(fp, 0, SEEK_END);
    size_t fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    Progress progress;
    progress.begin(adjfn, fsize);
    size_t fpos = 0;
    int edge_counter = 0;

    node_t cur_u = BAD_NODE;
    for (;;) {
        node_t v = BAD_NODE;
        int r = fread(&v, 1, sizeof(node_t), fp);
        if (r < sizeof(node_t) || feof(fp)) {
            break;
        }

        if (cur_u == BAD_NODE && v == BAD_NODE) {
            printf("  *** Unexpected code reached!\n");
            abort();
        } else if (cur_u == BAD_NODE && v != BAD_NODE) {
            // new src node found!
            cur_u = v;
        } else if (cur_u != BAD_NODE && v == BAD_NODE) {
            // new separator found!
            cur_u = BAD_NODE;

            size_t new_fpos = ftell(fp);
            progress.step(new_fpos - fpos);
            fpos = new_fpos;
        } else if (cur_u != BAD_NODE && v != BAD_NODE) {
            cb(edge_t { cur_u, v });
            edge_counter++;
        }
    }
    fclose(fp);

    timer.stop();
    printf("Loaded %d edges in %.6lf seconds.\n",
           edge_counter, timer.elapsed());
}

void load_txtfile(const char* txtfn, std::function<void(edge_t)>&& cb) {
    Timer timer;
    timer.start();

    std::ifstream fin(txtfn);
    std::string line;
    int edge_counter = 0;

    while (std::getline(fin, line)) {
        node_t u = -1, v = -1;
        std::istringstream iss(line);
        iss >> u;
        iss.get();  // skip separator (one char)
        iss >> v;
        if (u == -1 || v == -1) {
            continue;
        }
        edge_counter++;
        if (edge_counter % (1000 * 1000) == 0) {
            printf("Loaded %d edges\n", edge_counter);
        }
        cb(edge_t { u, v });
    }
    timer.stop();
    printf("Loaded %d edges in %.6lf seconds.\n",
           edge_counter, timer.elapsed());
}

void load_edge_file(const char* fpath, std::function<void(edge_t)>&& cb) {
    bool txt_only = true;

    FILE* fp = fopen(fpath, "rb");
    if (fp == nullptr) {
        printf("  *** Failed to open for read: %s\n", fpath);
        abort();
    }
    const int buf_size = 4096;
    char buf[buf_size];
    int cnt = fread(buf, 1, buf_size, fp);
    fclose(fp);

    for (int i = 0; i < cnt; i++) {
        if (!isprint(buf[i]) && !isspace(buf[i])) {
            txt_only = false;
            break;
        }
    }

    if (txt_only) {
        printf("Open as text file: %s\n", fpath);
        load_txtfile(fpath, std::move(cb));
    } else {
        printf("Open as adj list file: %s\n", fpath);
        load_adjfile(fpath, std::move(cb));
    }
}
