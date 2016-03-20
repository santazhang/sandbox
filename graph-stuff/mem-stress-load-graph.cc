#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>

#include <stdio.h>
#include <inttypes.h>
#include <unistd.h>

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

using namespace std;
using namespace google;

using node_t = int32_t;
using edge_t = std::pair<int32_t, int32_t>;

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

void proc_report() {
    ostringstream oss;
    oss << "/proc/" << ::getpid() << "/status";
    string proc_fn = oss.str();
    ifstream fin(proc_fn);
    if (fin) {
        string line;
        while (getline(fin, line)) {
            if (line.substr(0, 6) == "VmRSS:") {
                printf("%s\n\n", line.c_str());
                return;
            }
        }
    } else {
        printf("  *** failed to open %s\n", proc_fn.c_str());
    }
}

edge_t parse_edge(const std::string& line) {
    edge_t e{ -1, -1 };
    if (line[0] == '#') {
        return e;
    }
    istringstream iss(line);
    char ignored_char;
    iss >> e.first >> ignored_char >> e.second;
    if (e.first == -1 || e.second == -1) {
        e.first = -1;
        e.second = -1;
    }
    return e;
}

void stress1(const char* fpath) {
    ifstream fin(fpath);
    string line;
    int line_counter = 0;

    sparse_hash_set<edge_t> all_edges;
    while (getline(fin, line)) {
        line_counter++;
        if (line_counter % (1000 * 1000) == 0) {
            printf("Processed %d lines\n", line_counter);
            proc_report();
        }
        edge_t e = parse_edge(line);
        all_edges.insert(e);
    }
    printf("=== stress1: load %ld edges into sparse_hash_set<pair<i32, i32>>\n", all_edges.size());
    proc_report();
}

void stress2(const char* fpath) {
    ifstream fin(fpath);
    string line;
    int line_counter = 0;

    sparse_hash_map<edge_t, int32_t> edge_to_i32;
    while (getline(fin, line)) {
        line_counter++;
        if (line_counter % (1000 * 1000) == 0) {
            printf("Processed %d lines\n", line_counter);
            proc_report();
        }
        edge_t e = parse_edge(line);
        edge_to_i32[e] = 2016;
    }
    printf("=== stress2: load %ld edges into sparse_hash_map<pair<i32, i32> -> i32>\n", edge_to_i32.size());
    proc_report();
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s graph-file\n", argv[0]);
        return 1;
    }
    const char* fpath = argv[1];
    stress1(fpath);
    stress2(fpath);
    return 0;
}
