#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <deque>

#include <stdio.h>
#include <inttypes.h>
#include <unistd.h>

#include "graph_utils.h"

using namespace std;
using namespace google;

void load_edge_from_adj_index(const std::string& adjfn,
                              const std::string& idxfn,
                              std::function<void(edge_t)>&& cb) {
    FILE* fp_adj = fopen(adjfn.c_str(), "rb");
    if (fp_adj == nullptr) {
        printf("  *** Failed to open for read: %s\n", adjfn.c_str());
    }
    FILE* fp_idx = fopen(idxfn.c_str(), "rb");
    if (fp_idx == nullptr) {
        printf("  *** Failed to open for read: %s\n", idxfn.c_str());
    }

    for (;;) {
        node_t u = BAD_NODE;
        size_t offst = ~0;
        int r = fread(&u, 1, sizeof(node_t), fp_idx);
        if (r < sizeof(node_t) || feof(fp_idx)) {
            break;
        }
        r = fread(&offst, 1, sizeof(size_t), fp_idx);
        if (r < sizeof(size_t) || feof(fp_idx)) {
            break;
        }

        fseek(fp_adj, offst, SEEK_SET);
        node_t v = BAD_NODE;
        int counter = 0;
        do {
            r = fread(&v, 1, sizeof(node_t), fp_adj);
            if (r < sizeof(node_t) || feof(fp_adj)) {
                break;
            }
            if (counter == 0 && u != v) {
                printf("  *** Bad index: first element in adj-list should be source node!\n");
                abort();
            } else if (counter > 0 && v != BAD_NODE) {
                cb(edge_t { u, v });
            }
            counter++;
        } while (v != BAD_NODE);
    }

    fclose(fp_adj);
    fclose(fp_idx);
}

void test_adj_index(const std::string& adjfn) {
    std::string adj_idx = adjfn + ".index";
    std::string revfn = adjfn + ".reverse";
    std::string rev_idx = revfn + ".index";

    deque<edge_t> edges_loaded_from_adj;
    load_edge_file(adjfn.c_str(), [&edges_loaded_from_adj] (edge_t e) {
        edges_loaded_from_adj.push_back(e);
    });
    std::sort(edges_loaded_from_adj.begin(), edges_loaded_from_adj.end());
    printf("Loaded %ld edges from %s\n", edges_loaded_from_adj.size(), adjfn.c_str());

    {
        deque<edge_t> edge_from_index;
        load_edge_from_adj_index(adjfn, adj_idx, [&edge_from_index] (edge_t e) {
            edge_from_index.push_back(e);
        });
        std::sort(edge_from_index.begin(), edge_from_index.end());
        printf("Loaded %ld edges by index %s\n", edge_from_index.size(), adj_idx.c_str());
        if (edges_loaded_from_adj == edge_from_index) {
            printf("PASS 1\n");
        } else {
            printf("FAIL 1\n");
        }
    }

    {
        deque<edge_t> edge_from_index;
        load_edge_from_adj_index(revfn, rev_idx, [&edge_from_index] (edge_t e) {
            edge_from_index.push_back(edge_t { e.second, e.first });  // reverse edge
        });
        std::sort(edge_from_index.begin(), edge_from_index.end());
        printf("Loaded %ld edges by index %s\n", edge_from_index.size(), rev_idx.c_str());
        if (edges_loaded_from_adj == edge_from_index) {
            printf("PASS 2\n");
        } else {
            printf("FAIL 2\n");
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Usage: " << argv[0] << " graph-adj-file" << endl;
        return 1;
    }
    test_adj_index(argv[1]);
    return 0;
}
