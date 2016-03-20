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

void test_eval_edge_clustering() {
    printf("TEST: %s\n", __PRETTY_FUNCTION__);
    sparse_hash_map<edge_t, int32_t> edge_to_i32;
    edge_to_i32.insert(make_pair(edge_t { 1, 2 }, 1001));
    edge_to_i32.insert(make_pair(edge_t { 1, 3 }, 1002));
    edge_to_i32.insert(make_pair(edge_t { 1, 4 }, 1003));
    edge_to_i32.insert(make_pair(edge_t { 2, 3 }, 1004));
    edge_to_i32.insert(make_pair(edge_t { 2, 4 }, 1002));
    edge_to_i32.insert(make_pair(edge_t { 3, 4 }, 1001));
    printf("Test graph:\n");
    for (const auto& it : edge_to_i32) {
        printf("  %d -> %d   [%d]\n",
               it.first.first, it.first.second, it.second);
    }
    printf("%ld edges\n", edge_to_i32.size());
    eval_edge_clustering(edge_to_i32);
    printf("---\n");
}

int main(int argc, char* argv[]) {
    test_eval_edge_clustering();
    return 0;
}
