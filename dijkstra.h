#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "graph_types.h"
#include <queue>
#include <limits>
#include <utility>
#include <vector>

inline std::vector<double> dijkstra(const Graph& graph, const int start)
{
    constexpr double INF = std::numeric_limits<double>::infinity();
    const int n = graph.size();
    std::vector<double> dist(n, INF);

    if (n == 0) return dist;

    dist[start] = 0;

    // Binary Heap
    using QueueElement = std::pair<double, int>;
    std::priority_queue<QueueElement, std::vector<QueueElement>, std::greater<>> pq;
    pq.emplace(0, start);

    while (!pq.empty()) {
        const double d = pq.top().first;
        const int u = pq.top().second;
        pq.pop();

        if (d > dist[u]) continue;

        for (const auto& edge : graph.adj[u]) {
            const int v = edge.to;
            const double w = edge.weight;
            if (dist[u] != INF && dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                pq.emplace(dist[v], v);
            }
        }
    }
    return dist;
}

#endif // DIJKSTRA_H