#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "graph_types.h"
#include <queue>
#include <limits>
#include <utility>
#include <vector>
#include <fstream>
#include <string>

inline std::vector<double> dijkstra(const Graph& graph, const int start, const std::string& log_filename = "dijkstra.log")
{
    constexpr double INF = std::numeric_limits<double>::infinity();
    const int n = graph.size();
    std::vector<double> dist(n, INF);

    if (n == 0) return dist;

    std::ofstream log_file(log_filename);
    if (!log_file.is_open()) {
        std::cout << "Failed to open dijkstra.log file!";
    }

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

        if (log_file.is_open()) {
            log_file << "COMPLETE, " << u << "\n";
        }

        for (const auto& edge : graph.adj[u]) {
            const int v = edge.to;
            const double w = edge.weight;

            if (dist[u] != INF && dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;

                if (log_file.is_open()) {
                    log_file << "RELAX, " << v << "\n";
                }

                pq.emplace(dist[v], v);
            }
        }
    }

    if (log_file.is_open()) {
        log_file.close();
    }

    return dist;
}

#endif // DIJKSTRA_H