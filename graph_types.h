#ifndef GRAPH_TYPES_H
#define GRAPH_TYPES_H

#include <vector>
#include <string>

struct Edge {
    int to;
    double weight;
    Edge(const int t, const double w) : to(t), weight(w) { }
};

class Graph {
public:
    const int n;
    std::string name = "";
    std::vector<std::vector<Edge>> adj;

    explicit Graph(const int vertices) : n(vertices), adj(vertices) { }

    void add_edge(const int u, const int v, const double weight) {
        adj[u].emplace_back(v, weight);
    }

    [[nodiscard]] int size() const { return n; }
};

#endif // GRAPH_TYPES_H