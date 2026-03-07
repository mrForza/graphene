#include <queue>
#include <limits>
#include <unordered_map>
#include <algorithm>

#include "../general/graph/adj_list.hpp"
#include "../interfaces/shortest_path.h"

namespace graph {
class DijkstraShortestPath
    : public ShortestPathAlgorithm<void, int, false> {
public:
    using Base = ShortestPathAlgorithm<void, int, false>;
    using GraphType = typename Base::GraphType;
    using Path = typename Base::Path;
    using Paths = typename Base::Paths;

    Paths FindShortestPathes(const GraphType& graph) const override {
        Paths result;
        if (graph.empty()) {
            return result;
        }

        auto vertices = graph.get_vertices();
        if (vertices.empty()) {
            return result;
        }
        std::size_t start = *std::min_element(vertices.begin(), vertices.end());

        using Distance = int;
        const Distance INF = std::numeric_limits<Distance>::max();

        std::unordered_map<std::size_t, Distance> dist;
        std::unordered_map<std::size_t, std::size_t> parent;

        for (auto v : vertices) {
            dist[v] = INF;
        }
        dist[start] = 0;

        using QueueItem = std::pair<Distance, std::size_t>;
        std::priority_queue<QueueItem, std::vector<QueueItem>, std::greater<QueueItem>> pq;
        pq.emplace(0, start);

        while (!pq.empty()) {
            auto [d, v] = pq.top();
            pq.pop();

            if (d > dist[v]) {
                continue;
            }

            for (const auto& neighbor : graph.get_neighbors(v)) {
                std::size_t to = neighbor.vertex_id;
                Distance w = neighbor.weight;
                if (w < 0) {
                    continue;
                }
                if (dist[v] != INF && dist[v] + w < dist[to]) {
                    dist[to] = dist[v] + w;
                    parent[to] = v;
                    pq.emplace(dist[to], to);
                }
            }
        }

        result.resize(vertices.size());
        for (std::size_t i = 0; i < vertices.size(); ++i) {
            std::size_t v = vertices[i];
            if (dist[v] == INF) {
                continue;
            }

            Path path;
            std::size_t cur = v;
            while (cur != start) {
                path.push_back(cur);
                auto it = parent.find(cur);
                if (it == parent.end()) {
                    break;
                }
                cur = it->second;
            }
            path.push_back(start);
            std::reverse(path.begin(), path.end());
            result[i] = std::move(path);
        }

        return result;
    }
};

} // namespace graph

