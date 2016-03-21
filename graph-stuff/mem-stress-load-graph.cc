#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <deque>
#include <map>
#include <unordered_map>

#include <stdio.h>
#include <inttypes.h>
#include <unistd.h>

#include "graph_utils.h"

using namespace std;
using namespace google;

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

void stress1(const char* fpath) {
    REPORT_FUNCTION_TIMING_AFTER_RETURN;

    sparse_hash_set<edge_t> all_edges;
    all_edges.resize(estimate_edges_in_file(fpath));
    load_edge_file(fpath, [&all_edges] (edge_t e) {
        all_edges.insert(e);
    });
    printf("=== stress1: load %ld edges into sparse_set<edge>\n", all_edges.size());
    proc_report();
}

void stress2(const char* fpath) {
    REPORT_FUNCTION_TIMING_AFTER_RETURN;

    sparse_hash_map<edge_t, int32_t> edge_to_i32;
    edge_to_i32.resize(estimate_edges_in_file(fpath));
    load_edge_file(fpath, [&edge_to_i32] (edge_t e) {
        edge_to_i32[e] = 2016;
    });
    printf("=== stress2: load %ld edges into sparse_map<edge -> val>\n", edge_to_i32.size());
    proc_report();
}

// This is very slow (~40mins on twitter-small.csv, also uses twice mem than stress2)
void stress3(const char* fpath) {
    REPORT_FUNCTION_TIMING_AFTER_RETURN;

    int edge_count = 0;
    sparse_hash_map<node_t, sparse_hash_map<node_t, int32_t>> graph;
    load_edge_file(fpath, [&graph, &edge_count] (edge_t e) {
        graph[e.first][e.second] = 2016;
        edge_count++;
    });
    printf("=== stress3: load %d edges into sparse_map<node -> sparse_map<node -> val>>\n", edge_count);
    proc_report();
}

// Uses 2/3 time of stress2, but 5x memory.
void stress4(const char* fpath) {
    REPORT_FUNCTION_TIMING_AFTER_RETURN;

    map<edge_t, int32_t> edge_to_i32;
    load_edge_file(fpath, [&edge_to_i32] (edge_t e) {
        edge_to_i32[e] = 2016;
    });
    printf("=== stress4: load %ld edges into map<edge -> val>\n", edge_to_i32.size());
    proc_report();
}

// Speed similar to stress2, but uses 5x memory.
void stress5(const char* fpath) {
    REPORT_FUNCTION_TIMING_AFTER_RETURN;

    unordered_map<edge_t, int32_t> edge_to_i32;
    load_edge_file(fpath, [&edge_to_i32] (edge_t e) {
        edge_to_i32[e] = 2016;
    });
    printf("=== stress5: load %ld edges into unordered_map<edge -> val>\n", edge_to_i32.size());
    proc_report();
}

void stress6(const char* fpath) {
    REPORT_FUNCTION_TIMING_AFTER_RETURN;

    vector<edge_t> edges;
    load_edge_file(fpath, [&edges] (edge_t e) {
        edges.push_back(e);
    });
    printf("=== stress6: load %ld edges into vector<edge>\n", edges.size());
    proc_report();
}

void stress7(const char* fpath) {
    REPORT_FUNCTION_TIMING_AFTER_RETURN;

    deque<edge_t> edges;
    load_edge_file(fpath, [&edges] (edge_t e) {
        edges.push_back(e);
    });
    printf("=== stress6: load %ld edges into vector<edge>\n", edges.size());
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
    // stress3(fpath);
    // stress4(fpath);
    // stress5(fpath);
    stress6(fpath);
    stress7(fpath);
    return 0;
}
