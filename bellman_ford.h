#ifndef BELLMAN_FORD_H
#define BELLMAN_FORD_H

#include "graph_types.h"
#include <limits>
#include <vector>

inline std::vector<double> bellman_ford(const Graph& graph, const int start)
{
    constexpr double INF = std::numeric_limits<double>::infinity();
    const int n = graph.size();
    std::vector<double> dist(n, INF);

    if (n == 0) return dist;

    dist[start] = 0;

    for (int i = 0; i < n - 1; ++i) {
        bool updated = false;
        for (int u = 0; u < n; ++u) {
            if (dist[u] == INF) continue;
            for (const auto& edge : graph.adj[u]) {
                const int v = edge.to;
                const double w = edge.weight;
                if (dist[u] + w < dist[v]) {
                    dist[v] = dist[u] + w;
                    updated = true;
                }
            }
        }
        if (!updated) break;
    }
    return dist;
}

#endif // BELLMAN_FORD_H