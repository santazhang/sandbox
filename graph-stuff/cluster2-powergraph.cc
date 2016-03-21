#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

#include <stdio.h>
#include <inttypes.h>
#include <unistd.h>

#include "graph_utils.h"

using namespace std;
using namespace google;

int32_t cluster_with_least_load(const vector<int32_t>& choices,
                                const sparse_hash_map<int32_t, int32_t>& load) {
    int32_t cluster = choices[0];
    int32_t min_load = 0;

    auto it = load.find(cluster);
    if (it == load.end()) {
        return cluster;
    } else {
        min_load = it->second;
    }

    for (size_t i = 1; i < choices.size(); i++) {
        int32_t new_choice = choices[i];
        auto it = load.find(new_choice);
        if (it != load.end()) {
            int32_t new_load = it->second;
            if (new_load < min_load) {
                cluster = new_choice;
                min_load = new_load;
            }
        }
    }
    return cluster;
}

void debug_print_cluster_set(const vector<int32_t>& arr) {
    printf("[ ");
    for (int32_t v : arr) {
        printf("%d ", v);
    }
    printf("]");
}

void insert_into_cluster_set(vector<int32_t>* arr, int32_t val) {
    auto lower = std::lower_bound(arr->begin(), arr->end(), val);
    if (lower == arr->end()) {
        arr->push_back(val);
    } else {
        arr->insert(lower, val);
    }
}

// should be faster than std::set_intersection
vector<int32_t> cluster_intersection(const vector<int32_t>& sorted1,
                                     const vector<int32_t>& sorted2) {
    vector<int32_t> intersection;
    if (sorted1.empty() || sorted2.empty() ||
        sorted1.back() < sorted2.front() ||
        sorted2.back() < sorted1.front()
       ) {
        return intersection;
    }

    int32_t lookup1_key = sorted1[0];
    vector<int32_t>::const_iterator cursor1 = sorted1.begin();
    vector<int32_t>::const_iterator cursor2 = sorted2.begin();
    while (cursor1 != sorted1.end() && cursor2 != sorted2.end()) {
        auto it2 = std::lower_bound(cursor2, sorted2.end(), lookup1_key);
        cursor2 = it2;  // advance cursor in sorted2
        if (cursor2 == sorted2.end()) {
            // no more intersection possible
            break;
        }

        if (lookup1_key == *cursor2) {
            // lookup1_key exists in both sorted1 and sorted2
            intersection.push_back(lookup1_key);  // insert in sorted order
            // ok, advance cursors to check next candidate
            cursor1++;
            cursor2++;
        } else {
            int32_t lookup2_key = *cursor2;
            // lookup1_key not in sorted2, next candidate should be at least `lookup2_key` in sorted1
            auto it1 = std::lower_bound(cursor1, sorted1.end(), lookup2_key);
            cursor1 = it1;  // advance cursor in sorted1
            if (cursor1 == sorted1.end()) {
                // no more intersection possible
                break;
            }
            lookup1_key = *cursor1;  // update the lookup key
        }
    }

    return intersection;
}

void powergraph_clustering(const char* fpath) {
    Timer timer;
    Progress progress;

    sparse_hash_map<edge_t, int32_t> edge_to_i32;
    edge_to_i32.resize(estimate_edges_in_file(fpath));
    load_edge_file(fpath, [&edge_to_i32] (edge_t e) {
        edge_to_i32[e] = -1;
    });

    sparse_hash_map<node_t, vector<int32_t>> node_to_clusters;
    node_to_clusters.resize(estimate_node_count(edge_to_i32));

    sparse_hash_map<int32_t, int32_t> cluster_to_load;
    cluster_to_load.resize(MAX_CLUSTERS);

    int32_t next_cluster_id = 0;

    // Powergraph clustering
    timer.start();
    progress.begin("Powergraph clustering", edge_to_i32.size());
    for (auto& it : edge_to_i32) {
        node_t u = it.first.first, v = it.first.second;
        int32_t assigned_cluster = -1;

        node_to_clusters[u].reserve(0);
        node_to_clusters[v].reserve(0);
        // NOTE: Code above ensures the following code works even if
        //       hash table has been rebalanced in the middle.
        vector<int32_t>& u_set = node_to_clusters[u];
        vector<int32_t>& v_set = node_to_clusters[v];

        if (u_set.empty() && v_set.empty()) {
            // Neither node assigned, assign to new cluster.
            assigned_cluster = next_cluster_id++;
            insert_into_cluster_set(&u_set, assigned_cluster);
            insert_into_cluster_set(&v_set, assigned_cluster);

        } else if (u_set.empty() && !v_set.empty()) {
            // Assign u to v's set
            assigned_cluster = cluster_with_least_load(v_set, cluster_to_load);
            insert_into_cluster_set(&u_set, assigned_cluster);

        } else if (!u_set.empty() && v_set.empty()) {
            // Assign v to u's set
            assigned_cluster = cluster_with_least_load(u_set, cluster_to_load);
            insert_into_cluster_set(&v_set, assigned_cluster);

        } else {
            // Both u and v have been assigned
            vector<int32_t> intersection = cluster_intersection(u_set, v_set);
            if (intersection.empty()) {
                int32_t choice_u = cluster_with_least_load(u_set, cluster_to_load);
                int32_t choice_v = cluster_with_least_load(v_set, cluster_to_load);
                assigned_cluster = cluster_with_least_load(
                    vector<int32_t> { choice_u, choice_v }, cluster_to_load);

            } else {
                assigned_cluster = cluster_with_least_load(intersection, cluster_to_load);
            }
        }
        cluster_to_load[assigned_cluster]++;
        it.second = assigned_cluster;  // put edge to assigned cluster

        progress.step();
    }
    timer.stop();
    printf("Powergraph clustering done in %.6lf seconds.\n", timer.elapsed());

    eval_edge_clustering(edge_to_i32);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s graph-txt-or-adj\n", argv[0]);
        return 1;
    }
    my_srand();
    const char* fpath = argv[1];
    powergraph_clustering(fpath);
    return 0;
}
