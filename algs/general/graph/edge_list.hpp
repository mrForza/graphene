#ifndef EDGE_LIST_HPP
#define EDGE_LIST_HPP

#include "graph.h"
#include <vector>
#include <unordered_set>
#include <algorithm>

namespace graph {

template<typename VertexData = void, typename EdgeWeight = int, bool Directed = false>
class EdgeList : public Graph<VertexData, EdgeWeight, Directed> {
public:
    using Base = Graph<VertexData, EdgeWeight, Directed>;
    using VertexType = typename Base::VertexType;
    using EdgeType = typename Base::EdgeType;
    using NeighborType = typename Base::NeighborType;

private:
    std::vector<EdgeType> edges_;
    std::unordered_set<size_t> vertices_;

    typename std::vector<EdgeType>::iterator find_edge(size_t from, size_t to) {
        return std::find_if(edges_.begin(), edges_.end(),
            [from, to](const EdgeType& e) {
                if constexpr (Directed) {
                    return e.from == from && e.to == to;
                } else {
                    return (e.from == from && e.to == to) || 
                           (e.from == to && e.to == from);
                }
            });
    }

    typename std::vector<EdgeType>::const_iterator find_edge(size_t from, size_t to) const {
        return std::find_if(edges_.begin(), edges_.end(),
            [from, to](const EdgeType& e) {
                if constexpr (Directed) {
                    return e.from == from && e.to == to;
                } else {
                    return (e.from == from && e.to == to) || 
                           (e.from == to && e.to == from);
                }
            });
    }

public:
    EdgeList() = default;

    ~EdgeList() override = default;

    size_t add_vertex(size_t vertex_id) override {
        vertices_.insert(vertex_id);
        return vertex_id;
    }

    bool remove_vertex(size_t vertex_id) override {
        if (vertices_.find(vertex_id) == vertices_.end()) {
            return false;
        }

        edges_.erase(
            std::remove_if(edges_.begin(), edges_.end(),
                [vertex_id](const EdgeType& e) {
                    return e.from == vertex_id || e.to == vertex_id;
                }),
            edges_.end()
        );

        vertices_.erase(vertex_id);
        return true;
    }

    bool has_vertex(size_t vertex_id) const override {
        return vertices_.find(vertex_id) != vertices_.end();
    }

    size_t num_vertices() const override {
        return vertices_.size();
    }

    std::vector<size_t> get_vertices() const override {
        return std::vector<size_t>(vertices_.begin(), vertices_.end());
    }

    bool add_edge(size_t from, size_t to, EdgeWeight weight = EdgeWeight{}) override {
        vertices_.insert(from);
        vertices_.insert(to);

        auto it = find_edge(from, to);
        bool edge_exists = (it != edges_.end());

        if (edge_exists) {
            it->weight = weight;
        } else {
            edges_.emplace_back(from, to, weight);
        }

        return !edge_exists;
    }

    bool remove_edge(size_t from, size_t to) override {
        auto it = find_edge(from, to);
        if (it == edges_.end()) {
            return false;
        }

        edges_.erase(it);
        return true;
    }

    bool has_edge(size_t from, size_t to) const override {
        return find_edge(from, to) != edges_.end();
    }

    EdgeWeight get_edge_weight(size_t from, size_t to) const override {
        auto it = find_edge(from, to);
        if (it == edges_.end()) {
            return EdgeWeight{};
        }
        return it->weight;
    }

    bool set_edge_weight(size_t from, size_t to, EdgeWeight weight) override {
        auto it = find_edge(from, to);
        if (it == edges_.end()) {
            return false;
        }

        it->weight = weight;
        return true;
    }

    size_t num_edges() const override {
        return edges_.size();
    }

    std::vector<EdgeType> get_edges() const override {
        return edges_;
    }

    std::vector<NeighborType> get_neighbors(size_t vertex_id) const override {
        std::vector<NeighborType> neighbors;

        for (const auto& edge : edges_) {
            if constexpr (Directed) {
                if (edge.from == vertex_id) {
                    neighbors.emplace_back(edge.to, edge.weight);
                }
            } else {
                if (edge.from == vertex_id) {
                    neighbors.emplace_back(edge.to, edge.weight);
                } else if (edge.to == vertex_id) {
                    neighbors.emplace_back(edge.from, edge.weight);
                }
            }
        }

        return neighbors;
    }

    size_t degree(size_t vertex_id) const override {
        if constexpr (Directed) {
            return in_degree(vertex_id) + out_degree(vertex_id);
        } else {
            return out_degree(vertex_id);
        }
    }

    size_t in_degree(size_t vertex_id) const override {
        size_t count = 0;
        for (const auto& edge : edges_) {
            if (edge.to == vertex_id) {
                count++;
            }
        }
        return count;
    }

    size_t out_degree(size_t vertex_id) const override {
        size_t count = 0;
        for (const auto& edge : edges_) {
            if (edge.from == vertex_id) {
                count++;
            }
        }
        return count;
    }

    bool empty() const override {
        return vertices_.empty();
    }

    void clear() override {
        edges_.clear();
        vertices_.clear();
    }
};

}

#endif // EDGE_LIST_HPP
