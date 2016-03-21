#include <iostream>
#include <string>
#include <deque>

#include "graph_utils.h"

using namespace std;
using namespace google;

void graph_adj2txt(const char* adjfn, const char* txtfn) {
    deque<edge_t> edges;

    Timer timer;
    timer.start();
    load_adjfile(adjfn, [&edges] (edge_t e) {
        edges.push_back(e);
    });
    timer.stop();
    printf("Loading %ld edges from adjfile takes %.6lf seconds.\n",
           edges.size(), timer.elapsed());

    timer.start();
    std::sort(edges.begin(), edges.end());
    timer.stop();
    printf("Sorted %ld edges in %.6lf seconds.\n",
           edges.size(), timer.elapsed());

    FILE* fp = fopen(txtfn, "w");
    if (fp == nullptr) {
        printf("  *** Failed to open for write: %s\n", txtfn);
        exit(1);
    }

    Progress progress;
    progress.begin(txtfn, edges.size());
    for (const auto& it : edges) {
        fprintf(fp, "%d %d\n", it.first, it.second);
        progress.step();
    }
    fclose(fp);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cout << "Usage: " << argv[0] << " graph-adj-file output-txt-file" << endl;
        return 1;
    }
    graph_adj2txt(argv[1], argv[2]);
    return 0;
}
