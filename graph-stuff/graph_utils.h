#pragma once

#include <utility>
#include <vector>
#include <algorithm>
#include <functional>
#include <string>

#include <stdio.h>
#include <inttypes.h>
#include <sys/time.h>

// http://stackoverflow.com/a/31798726/1035246
// Workaround missing "is_trivially_copyable" in g++ < 5.0
#if __GNUG__ && __GNUC__ < 5
#define IS_TRIVIALLY_COPYABLE(T) __has_trivial_copy(T)
namespace std {

template <typename T>
struct is_trivially_copy_constructible {
    static const bool value = __has_trivial_copy(T);
};

}  // namespace std
#endif

#include "sparsehash/sparse_hash_map"
#include "sparsehash/sparse_hash_set"


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


namespace std {

template <>
struct hash<edge_t> {
    size_t operator() (edge_t e) const {
        return (h_(e.first) * 0x9e3779b97f4a7c15LLU) ^ h_(e.second);
    }

private:
    hash<node_t> h_;
};

}  // namespace std
