#ifndef GRAPH_H
#define GRAPH_H

#include <cstddef>
#include <vector>
#include <utility>

namespace graph {

template<typename VertexData = void, typename EdgeWeight = int>
class Graph;

template<typename EdgeWeight = int>
struct Edge {
    size_t from;
    size_t to;
    EdgeWeight weight;

    Edge(size_t from, size_t to, EdgeWeight weight = EdgeWeight{})
        : from(from), to(to), weight(weight) {}

    bool operator==(const Edge<EdgeWeight>& other) const {
        return from == other.from && to == other.to && weight == other.weight;
    }

    bool operator!=(const Edge<EdgeWeight>& other) const {
        return !(*this == other);
    }
};

template<typename VertexData = void>
struct Vertex {
    size_t id;
    VertexData data;

-    explicit Vertex(size_t id) : id(id), data{} {}

    template<typename T = VertexData>
    explicit Vertex(size_t id, typename std::enable_if<!std::is_void<T>::value, const T&>::type data)
        : id(id), data(data) {}
};

// Specialization for void vertex data
template<>
struct Vertex<void> {
    size_t id;

    explicit Vertex(size_t id) : id(id) {}
};

template<typename EdgeWeight = int>
struct Neighbor {
    size_t vertex_id;   // Neighbor vertex ID
    EdgeWeight weight;  // Edge weight

    Neighbor(size_t vertex_id, EdgeWeight weight = EdgeWeight{})
        : vertex_id(vertex_id), weight(weight) {}

    bool operator==(const Neighbor<EdgeWeight>& other) const {
        return vertex_id == other.vertex_id && weight == other.weight;
    }
};

template<typename VertexData = void, typename EdgeWeight = int, bool Directed = false>
class Graph {
public:
    using VertexType = Vertex<VertexData>;
    using EdgeType = Edge<EdgeWeight>;
    using NeighborType = Neighbor<EdgeWeight>;

    virtual ~Graph() = default;

    virtual size_t add_vertex(size_t vertex_id) = 0;

    template<typename T = VertexData>
    typename std::enable_if<!std::is_void<T>::value, size_t>::type
    add_vertex(size_t vertex_id, const T& data) {
        return add_vertex(vertex_id);
    }

    virtual bool remove_vertex(size_t vertex_id) = 0;

    virtual bool has_vertex(size_t vertex_id) const = 0;

    virtual size_t num_vertices() const = 0;

    virtual std::vector<size_t> get_vertices() const = 0;

    virtual bool add_edge(size_t from, size_t to, EdgeWeight weight = EdgeWeight{}) = 0;

    virtual bool remove_edge(size_t from, size_t to) = 0;

    virtual bool has_edge(size_t from, size_t to) const = 0;

    virtual EdgeWeight get_edge_weight(size_t from, size_t to) const = 0;

    virtual bool set_edge_weight(size_t from, size_t to, EdgeWeight weight) = 0;

    virtual size_t num_edges() const = 0;

    virtual std::vector<EdgeType> get_edges() const = 0;

    virtual std::vector<NeighborType> get_neighbors(size_t vertex_id) const = 0;

    virtual size_t degree(size_t vertex_id) const = 0;

    virtual size_t in_degree(size_t vertex_id) const = 0;

    virtual size_t out_degree(size_t vertex_id) const = 0;

    virtual bool empty() const = 0;

    virtual void clear() = 0;

    static constexpr bool is_directed() {
        return Directed;
    }
};

}

#endif // GRAPH_H
