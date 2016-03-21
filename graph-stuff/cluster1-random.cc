#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>

#include <stdio.h>
#include <inttypes.h>
#include <unistd.h>

#include "graph_utils.h"

using namespace std;
using namespace google;

edge_t parse_edge(const std::string& line) {
    edge_t e { -1, -1 };
    if (line[0] == '#') {
        return e;
    }
    istringstream iss(line);
    iss >> e.first;
    iss.get();  // skip separator (one char)
    iss >> e.second;
    if (e.first == -1 || e.second == -1) {
        e.first = -1;
        e.second = -1;
    }
    return e;
}

void random_clustering(const char* fpath) {
    Timer timer;
    Progress progress;

    sparse_hash_map<edge_t, int32_t> edge_to_i32;
    edge_to_i32.resize(estimate_edges_in_txt_file(fpath));
    load_edge_file(fpath, [&edge_to_i32] (edge_t e) {
        edge_to_i32[e] = 2016;
    });

    // random clustering
    timer.start();
    progress.begin("Random clustering", edge_to_i32.size());
    for (auto& it : edge_to_i32) {
        it.second = my_rand() % MAX_CLUSTERS;
        progress.step();
    }
    timer.stop();
    printf("Random clustering done in %.6lf seconds.\n", timer.elapsed());

    eval_edge_clustering(edge_to_i32);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s graph-txt-or-adj\n", argv[0]);
        return 1;
    }
    my_srand();
    const char* fpath = argv[1];
    random_clustering(fpath);
    return 0;
}
