#ifndef ADJ_LIST_HPP
#define ADJ_LIST_HPP

#include "graph.h"
#include <unordered_map>
#include <vector>
#include <algorithm>

namespace graph {

template<typename VertexData = void, typename EdgeWeight = int, bool Directed = false>
class AdjacencyList : public Graph<VertexData, EdgeWeight, Directed> {
public:
    using Base = Graph<VertexData, EdgeWeight, Directed>;
    using VertexType = typename Base::VertexType;
    using EdgeType = typename Base::EdgeType;
    using NeighborType = typename Base::NeighborType;

private:
    std::unordered_map<size_t, std::vector<NeighborType>> adj_list_;
    size_t num_edges_;

    typename std::vector<NeighborType>::iterator find_neighbor(
        std::vector<NeighborType>& neighbors, size_t neighbor_id) {
        return std::find_if(neighbors.begin(), neighbors.end(),
            [neighbor_id](const NeighborType& n) {
                return n.vertex_id == neighbor_id;
            });
    }

    typename std::vector<NeighborType>::const_iterator find_neighbor(
        const std::vector<NeighborType>& neighbors, size_t neighbor_id) const {
        return std::find_if(neighbors.begin(), neighbors.end(),
            [neighbor_id](const NeighborType& n) {
                return n.vertex_id == neighbor_id;
            });
    }

public:
    AdjacencyList() : num_edges_(0) {}

    ~AdjacencyList() override = default;

    size_t add_vertex(size_t vertex_id) override {
        if (adj_list_.find(vertex_id) == adj_list_.end()) {
            adj_list_[vertex_id] = std::vector<NeighborType>();
        }
        return vertex_id;
    }

    bool remove_vertex(size_t vertex_id) override {
        auto it = adj_list_.find(vertex_id);
        if (it == adj_list_.end()) {
            return false;
        }

        size_t edges_removed = it->second.size();

        for (auto& pair : adj_list_) {
            if (pair.first != vertex_id) {
                auto neighbor_it = find_neighbor(pair.second, vertex_id);
                if (neighbor_it != pair.second.end()) {
                    pair.second.erase(neighbor_it);
                    if constexpr (!Directed) {
                        edges_removed--;
                    } else {
                        edges_removed++;
                    }
                }
            }
        }

        num_edges_ -= edges_removed;
        adj_list_.erase(it);

        return true;
    }

    bool has_vertex(size_t vertex_id) const override {
        return adj_list_.find(vertex_id) != adj_list_.end();
    }

    size_t num_vertices() const override {
        return adj_list_.size();
    }

    std::vector<size_t> get_vertices() const override {
        std::vector<size_t> vertices;
        vertices.reserve(adj_list_.size());
        for (const auto& pair : adj_list_) {
            vertices.push_back(pair.first);
        }
        return vertices;
    }

    bool add_edge(size_t from, size_t to, EdgeWeight weight = EdgeWeight{}) override {
        if (!has_vertex(from)) {
            add_vertex(from);
        }
        if (!has_vertex(to)) {
            add_vertex(to);
        }

        auto& from_neighbors = adj_list_[from];
        auto neighbor_it = find_neighbor(from_neighbors, to);

        bool edge_exists = (neighbor_it != from_neighbors.end());

        if (edge_exists) {
            neighbor_it->weight = weight;
        } else {
            from_neighbors.emplace_back(to, weight);
            num_edges_++;
        }

        if constexpr (!Directed) {
            if (from != to) {
                auto& to_neighbors = adj_list_[to];
                auto reverse_it = find_neighbor(to_neighbors, from);
                if (reverse_it == to_neighbors.end()) {
                    to_neighbors.emplace_back(from, weight);
                } else {
                    reverse_it->weight = weight;
                }
            }
        }

        return !edge_exists;
    }

    bool remove_edge(size_t from, size_t to) override {
        auto from_it = adj_list_.find(from);
        if (from_it == adj_list_.end()) {
            return false;
        }

        auto neighbor_it = find_neighbor(from_it->second, to);
        if (neighbor_it == from_it->second.end()) {
            return false;
        }

        from_it->second.erase(neighbor_it);
        num_edges_--;

        if constexpr (!Directed) {
            if (from != to) {
                auto to_it = adj_list_.find(to);
                if (to_it != adj_list_.end()) {
                    auto reverse_it = find_neighbor(to_it->second, from);
                    if (reverse_it != to_it->second.end()) {
                        to_it->second.erase(reverse_it);
                    }
                }
            }
        }

        return true;
    }

    bool has_edge(size_t from, size_t to) const override {
        auto it = adj_list_.find(from);
        if (it == adj_list_.end()) {
            return false;
        }

        return find_neighbor(it->second, to) != it->second.end();
    }

    EdgeWeight get_edge_weight(size_t from, size_t to) const override {
        auto it = adj_list_.find(from);
        if (it == adj_list_.end()) {
            return EdgeWeight{};
        }

        auto neighbor_it = find_neighbor(it->second, to);
        if (neighbor_it == it->second.end()) {
            return EdgeWeight{};
        }

        return neighbor_it->weight;
    }

    bool set_edge_weight(size_t from, size_t to, EdgeWeight weight) override {
        if (!has_edge(from, to)) {
            return false;
        }

        auto& from_neighbors = adj_list_[from];
        auto neighbor_it = find_neighbor(from_neighbors, to);
        neighbor_it->weight = weight;

        if constexpr (!Directed) {
            if (from != to) {
                auto& to_neighbors = adj_list_[to];
                auto reverse_it = find_neighbor(to_neighbors, from);
                if (reverse_it != to_neighbors.end()) {
                    reverse_it->weight = weight;
                }
            }
        }

        return true;
    }

    size_t num_edges() const override {
        return num_edges_;
    }

    std::vector<EdgeType> get_edges() const override {
        std::vector<EdgeType> edges;
        edges.reserve(num_edges_);

        for (const auto& pair : adj_list_) {
            size_t from = pair.first;
            for (const auto& neighbor : pair.second) {
                if constexpr (Directed) {
                    edges.emplace_back(from, neighbor.vertex_id, neighbor.weight);
                } else {
                    if (from <= neighbor.vertex_id) {
                        edges.emplace_back(from, neighbor.vertex_id, neighbor.weight);
                    }
                }
            }
        }

        return edges;
    }

    std::vector<NeighborType> get_neighbors(size_t vertex_id) const override {
        auto it = adj_list_.find(vertex_id);
        if (it == adj_list_.end()) {
            return std::vector<NeighborType>();
        }

        return it->second;
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
        for (const auto& pair : adj_list_) {
            if (pair.first != vertex_id) {
                for (const auto& neighbor : pair.second) {
                    if (neighbor.vertex_id == vertex_id) {
                        count++;
                    }
                }
            }
        }
        return count;
    }

    size_t out_degree(size_t vertex_id) const override {
        auto it = adj_list_.find(vertex_id);
        if (it == adj_list_.end()) {
            return 0;
        }
        return it->second.size();
    }

    bool empty() const override {
        return adj_list_.empty();
    }

    void clear() override {
        adj_list_.clear();
        num_edges_ = 0;
    }
};

}

#endif // ADJ_LIST_HPP
