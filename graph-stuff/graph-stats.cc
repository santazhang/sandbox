#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

using namespace std;

struct node_deg {
    int in {0};
    int out {0};
};

void print_degree_distribution(
        const unordered_map<int, node_deg>& deg,
        std::function<int(node_deg)>&& func) {
    vector<int> dist;
    for (const auto& it : deg) {
        int d = func(it.second);
        if (dist.size() < d + 1) {
            dist.resize(d + 1);
        }
        dist[d]++;
    }
    for (size_t i = 0; i < dist.size(); i++) {
        if (dist[i] != 0) {
            cout << "  deg=" << i << ": " << dist[i] << endl;
        }
    }
}

void analyze_graph_file(const char* fpath) {
    ifstream fin(fpath);
    string line;
    int edge_counter = 0;
    unordered_map<int, node_deg> deg;

    while (getline(fin, line)) {
        int u = -1, v = -1;
        istringstream iss(line);
        iss >> u;
        iss.get();  // skip separator (one char)
        iss >> v;
        if (u == -1 || v == -1) {
            continue;
        }
        edge_counter++;
        if (edge_counter % (1000 * 1000) == 0) {
            cout << "Analyzed " << edge_counter << " edges" << endl;
        }
        node_deg& deg_u = deg[u];
        node_deg& deg_v = deg[v];
        deg_u.out++;
        deg_v.in++;
    }
    cout << "---" << endl;
    cout << "Nodes: " << deg.size() << endl;
    cout << "Edges: " << edge_counter << endl;
    cout << "Indegree distribution:" << endl;
    print_degree_distribution(deg, [] (node_deg d) { return d.in; });
    cout << "Outdegree distribution:" << endl;
    print_degree_distribution(deg, [] (node_deg d) { return d.out; });
    cout << "Degree distribution:" << endl;
    print_degree_distribution(deg, [] (node_deg d) { return d.in + d.out; });
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Usage: " << argv[0] << " graph-file" << endl;
        return 1;
    }
    analyze_graph_file(argv[1]);
    return 0;
}
