// adjfile format:
// all little-endian int32_t
// <src> <tgt1> <tgt2> ... <tgt last>  <separator = -1>
// both src and tgt are sorted.
// last separator might be missing.
//
// adj index format:
// <src> <8byte offst>
// <offset> point to <src> in adj file

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <deque>
#include <utility>
#include <algorithm>

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

#include "graph_utils.h"

using namespace std;

void write_adjfile(const deque<edge_t>& edges, const std::string& adjfn) {
    Timer timer;

    timer.start();
    FILE* fp = fopen(adjfn.c_str(), "wb");
    if (fp == nullptr) {
        printf("  *** Failed to open for write: %s\n", adjfn.c_str());
        exit(1);
    }

    int write_edge = 0;
    int write_src_node = 0;
    node_t last_u = -1;
    for (const auto& it : edges) {
        node_t u = it.first, v = it.second;
        if (u != last_u) {
            if (last_u != -1) {
                fwrite(&BAD_NODE, 1, sizeof(node_t), fp);  // separator
            }
            fwrite(&u, 1, sizeof(node_t), fp);  // <src>
            write_src_node++;
        }
        fwrite(&v, 1, sizeof(node_t), fp);  // <tgt>
        write_edge++;
        last_u = u;
    }

    fclose(fp);

    timer.stop();
    printf("Written %d edges from %d source nodes in %.6lf seconds.\n",
           write_edge, write_src_node, timer.elapsed());
}

void build_adj_index(const std::string& adjfn) {
    Timer timer;
    timer.start();

    FILE* fp_in = fopen(adjfn.c_str(), "rb");
    if (fp_in == NULL) {
        printf("  *** Failed to open for read: %s\n", adjfn.c_str());
        exit(1);
    }
    std::string idxfn = adjfn + ".index";
    FILE* fp_out = fopen(idxfn.c_str(), "wb");
    if (fp_out == NULL) {
        printf("  *** Failed to open for write: %s\n", idxfn.c_str());
        exit(1);
    }

    size_t offst = 0;
    int read_node = 0;
    int read_edge = 0;
    node_t cur_src_node = BAD_NODE;
    for (;;) {
        node_t u = BAD_NODE;
        int r = fread(&u, 1, sizeof(node_t), fp_in);
        if (r < sizeof(node_t) || feof(fp_in)) {
            break;
        }

        if (cur_src_node == BAD_NODE && u != BAD_NODE) {
            // we are looking for new src node
            cur_src_node = u;
            fwrite(&u, 1, sizeof(node_t), fp_out);  // write the index
            fwrite(&offst, 1, sizeof(size_t), fp_out);
            read_node++;
        } else if (cur_src_node == BAD_NODE && u == BAD_NODE) {
            printf("  *** Unexpected code reached!\n");
            abort();
        } else if (cur_src_node != BAD_NODE && u != BAD_NODE) {
            // got edge
            read_edge++;
        } else if (cur_src_node != BAD_NODE && u == BAD_NODE) {
            // got separator, prepare for next src node
            cur_src_node = BAD_NODE;
        }
        offst += r;
    }

    fclose(fp_in);
    fclose(fp_out);

    timer.stop();
    printf("Indexed %d edges from %d source nodes in %.6lf seconds.\n",
           read_edge, read_node, timer.elapsed());
}

void graph_txt2adj(const char* txtfn, const std::string& adjfn) {
    Timer timer;
    deque<edge_t> edges;

    /*
    timer.start();
    ifstream fin(txtfn);
    string line;
    int edge_counter = 0;
    

    while (getline(fin, line)) {
        node_t u = -1, v = -1;
        istringstream iss(line);
        iss >> u;
        iss.get();  // skip separator (one char)
        iss >> v;
        if (u == -1 || v == -1) {
            continue;
        }
        edge_counter++;
        if (edge_counter % (1000 * 1000) == 0) {
            cout << "Loaded " << edge_counter << " edges" << endl;
        }
        edge_t e { u, v };
        edges.push_back(e);
    }
    timer.stop();
    printf("Loaded %ld edges in %.6lf seconds.\n",
           edges.size(), timer.elapsed());
    */
    load_txtfile(txtfn, [&edges] (edge_t e) {
        edges.push_back(e);
    });

    timer.start();
    std::sort(edges.begin(), edges.end());
    timer.stop();
    printf("Sorted %ld edges in %.6lf seconds.\n",
           edges.size(), timer.elapsed());

    write_adjfile(edges, adjfn);
    build_adj_index(adjfn);

    timer.start();
    for (auto& it : edges) {
        it = edge_t { it.second, it.first };
    }
    timer.stop();
    printf("Reversed %ld edges in %.6lf seconds.\n",
           edges.size(), timer.elapsed());

    timer.start();
    std::sort(edges.begin(), edges.end());
    timer.stop();
    printf("Sorted %ld edges in %.6lf seconds.\n",
           edges.size(), timer.elapsed());

    std::string revfn = adjfn + ".reverse";
    write_adjfile(edges, revfn);
    build_adj_index(revfn);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cout << "Usage: " << argv[0] << " graph-txt-file output-adj-file" << endl;
        return 1;
    }
    graph_txt2adj(argv[1], argv[2]);
    return 0;
}
