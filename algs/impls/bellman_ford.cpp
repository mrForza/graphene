#include <limits>
#include <unordered_map>
#include <algorithm>

#include "../general/graph/edge_list.hpp"
#include "../interfaces/shortest_path.h"

namespace graph {
class BellmanFordShortestPath
    : public ShortestPathAlgorithm<void, int, true> {
public:
    using Base = ShortestPathAlgorithm<void, int, true>;
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

        auto edges = graph.get_edges();
        std::size_t n = vertices.size();

        for (std::size_t i = 0; i < n - 1; ++i) {
            bool changed = false;
            for (const auto& e : edges) {
                if (dist[e.from] == INF) {
                    continue;
                }
                if (dist[e.from] + e.weight < dist[e.to]) {
                    dist[e.to] = dist[e.from] + e.weight;
                    parent[e.to] = e.from;
                    changed = true;
                }
            }
            if (!changed) {
                break;
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

