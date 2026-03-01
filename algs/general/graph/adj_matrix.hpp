#ifndef ADJ_MATRIX_HPP
#define ADJ_MATRIX_HPP

#include "graph.h"
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <limits>

namespace graph {

template<typename VertexData = void, typename EdgeWeight = int, bool Directed = false>
class AdjacencyMatrix : public Graph<VertexData, EdgeWeight, Directed> {
public:
    using Base = Graph<VertexData, EdgeWeight, Directed>;
    using VertexType = typename Base::VertexType;
    using EdgeType = typename Base::EdgeType;
    using NeighborType = typename Base::NeighborType;

private:
    std::vector<std::vector<EdgeWeight>> matrix_;
    std::unordered_map<size_t, size_t> vertex_map_;
    std::unordered_map<size_t, size_t> index_map_;
    size_t num_edges_;
    size_t next_index_;

    static constexpr EdgeWeight NO_EDGE = std::numeric_limits<EdgeWeight>::max();

    size_t get_matrix_index(size_t vertex_id) const {
        auto it = vertex_map_.find(vertex_id);
        return (it != vertex_map_.end()) ? it->second : SIZE_MAX;
    }

    void resize_matrix(size_t new_size) {
        size_t old_size = matrix_.size();
        matrix_.resize(new_size);
        
        for (size_t i = 0; i < new_size; ++i) {
            matrix_[i].resize(new_size, NO_EDGE);
        }
    }

public:
    AdjacencyMatrix() : num_edges_(0), next_index_(0) {}

    explicit AdjacencyMatrix(size_t initial_size) 
        : num_edges_(0), next_index_(0) {
        resize_matrix(initial_size);
    }

    ~AdjacencyMatrix() override = default;

    size_t add_vertex(size_t vertex_id) override {
        if (has_vertex(vertex_id)) {
            return vertex_id;
        }

        size_t index = next_index_++;
        vertex_map_[vertex_id] = index;
        index_map_[index] = vertex_id;

        if (index >= matrix_.size()) {
            resize_matrix(index + 1);
        }

        return vertex_id;
    }

    bool remove_vertex(size_t vertex_id) override {
        size_t index = get_matrix_index(vertex_id);
        if (index == SIZE_MAX) {
            return false;
        }

        size_t edges_removed = 0;
        for (size_t i = 0; i < matrix_.size(); ++i) {
            if (matrix_[index][i] != NO_EDGE) edges_removed++;
            if (matrix_[i][index] != NO_EDGE) edges_removed++;
        }

        if (matrix_[index][index] != NO_EDGE) edges_removed--;

        for (size_t i = 0; i < matrix_.size(); ++i) {
            matrix_[index][i] = NO_EDGE;
            matrix_[i][index] = NO_EDGE;
        }

        num_edges_ -= edges_removed;

        vertex_map_.erase(vertex_id);
        index_map_.erase(index);

        return true;
    }

    bool has_vertex(size_t vertex_id) const override {
        return vertex_map_.find(vertex_id) != vertex_map_.end();
    }

    size_t num_vertices() const override {
        return vertex_map_.size();
    }

    std::vector<size_t> get_vertices() const override {
        std::vector<size_t> vertices;
        vertices.reserve(vertex_map_.size());
        for (const auto& pair : vertex_map_) {
            vertices.push_back(pair.first);
        }
        return vertices;
    }


    bool add_edge(size_t from, size_t to, EdgeWeight weight = EdgeWeight{}) override {
        size_t from_idx = get_matrix_index(from);
        size_t to_idx = get_matrix_index(to);

        if (from_idx == SIZE_MAX || to_idx == SIZE_MAX) {
            return false;
        }

        bool edge_exists = (matrix_[from_idx][to_idx] != NO_EDGE);
        
        if (!edge_exists) {
            num_edges_++;
        }

        matrix_[from_idx][to_idx] = weight;

        if (!Directed && from != to) {
            matrix_[to_idx][from_idx] = weight;
        }

        return !edge_exists;
    }

    bool remove_edge(size_t from, size_t to) override {
        size_t from_idx = get_matrix_index(from);
        size_t to_idx = get_matrix_index(to);

        if (from_idx == SIZE_MAX || to_idx == SIZE_MAX) {
            return false;
        }

        if (matrix_[from_idx][to_idx] == NO_EDGE) {
            return false;
        }

        matrix_[from_idx][to_idx] = NO_EDGE;
        num_edges_--;

        if (!Directed && from != to) {
            matrix_[to_idx][from_idx] = NO_EDGE;
        }

        return true;
    }

    bool has_edge(size_t from, size_t to) const override {
        size_t from_idx = get_matrix_index(from);
        size_t to_idx = get_matrix_index(to);

        if (from_idx == SIZE_MAX || to_idx == SIZE_MAX) {
            return false;
        }

        return matrix_[from_idx][to_idx] != NO_EDGE;
    }

    EdgeWeight get_edge_weight(size_t from, size_t to) const override {
        size_t from_idx = get_matrix_index(from);
        size_t to_idx = get_matrix_index(to);

        if (from_idx == SIZE_MAX || to_idx == SIZE_MAX) {
            return EdgeWeight{};
        }

        if (matrix_[from_idx][to_idx] == NO_EDGE) {
            return EdgeWeight{};
        }

        return matrix_[from_idx][to_idx];
    }

    bool set_edge_weight(size_t from, size_t to, EdgeWeight weight) override {
        if (!has_edge(from, to)) {
            return false;
        }

        size_t from_idx = get_matrix_index(from);
        size_t to_idx = get_matrix_index(to);

        matrix_[from_idx][to_idx] = weight;

        if (!Directed && from != to) {
            matrix_[to_idx][from_idx] = weight;
        }

        return true;
    }

    size_t num_edges() const override {
        return num_edges_;
    }

    std::vector<EdgeType> get_edges() const override {
        std::vector<EdgeType> edges;
        edges.reserve(num_edges_);

        for (const auto& from_pair : vertex_map_) {
            size_t from_id = from_pair.first;
            size_t from_idx = from_pair.second;

            for (const auto& to_pair : vertex_map_) {
                size_t to_id = to_pair.first;
                size_t to_idx = to_pair.second;

                if (matrix_[from_idx][to_idx] != NO_EDGE) {
                    if (Directed || from_id <= to_id) {
                        edges.emplace_back(from_id, to_id, matrix_[from_idx][to_idx]);
                    }
                }
            }
        }

        return edges;
    }

    std::vector<NeighborType> get_neighbors(size_t vertex_id) const override {
        std::vector<NeighborType> neighbors;
        size_t idx = get_matrix_index(vertex_id);

        if (idx == SIZE_MAX) {
            return neighbors;
        }

        for (const auto& pair : index_map_) {
            size_t neighbor_idx = pair.first;
            size_t neighbor_id = pair.second;

            if (matrix_[idx][neighbor_idx] != NO_EDGE) {
                neighbors.emplace_back(neighbor_id, matrix_[idx][neighbor_idx]);
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
        size_t idx = get_matrix_index(vertex_id);
        if (idx == SIZE_MAX) {
            return 0;
        }

        size_t count = 0;
        for (size_t i = 0; i < matrix_.size(); ++i) {
            if (matrix_[i][idx] != NO_EDGE) {
                count++;
            }
        }

        return count;
    }

    size_t out_degree(size_t vertex_id) const override {
        size_t idx = get_matrix_index(vertex_id);
        if (idx == SIZE_MAX) {
            return 0;
        }

        size_t count = 0;
        for (size_t i = 0; i < matrix_.size(); ++i) {
            if (matrix_[idx][i] != NO_EDGE) {
                count++;
            }
        }

        return count;
    }

    bool empty() const override {
        return vertex_map_.empty();
    }

    void clear() override {
        matrix_.clear();
        vertex_map_.clear();
        index_map_.clear();
        num_edges_ = 0;
        next_index_ = 0;
    }
};

}

#endif // ADJ_MATRIX_HPP
