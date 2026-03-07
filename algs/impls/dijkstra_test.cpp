#include <gtest/gtest.h>
#include <numeric>

#include "../general/graph/adj_list.hpp"
#include "../interfaces/shortest_path.h"
#include "dijkstra.cpp"

using namespace graph;

namespace {

int PathDistance(const Graph<void, int, false>& graph,
                 const ShortestPathAlgorithm<void, int, false>::Path& path) {
    int sum = 0;
    for (std::size_t i = 0; i + 1 < path.size(); ++i) {
        sum += graph.get_edge_weight(path[i], path[i + 1]);
    }
    return sum;
}

const ShortestPathAlgorithm<void, int, false>::Path*
FindPathToVertex(const ShortestPathAlgorithm<void, int, false>::Paths& paths,
                 std::size_t target) {
    for (const auto& p : paths) {
        if (!p.empty() && p.back() == target) {
            return &p;
        }
    }
    return nullptr;
}

} // namespace

TEST(DijkstraShortestPathTest, SimpleLineGraph) {
    AdjacencyList<void, int, false> g;
    g.add_vertex(0);
    g.add_vertex(1);
    g.add_vertex(2);
    g.add_edge(0, 1, 1);
    g.add_edge(1, 2, 2);

    DijkstraShortestPath algo;
    auto paths = algo.FindShortestPathes(g);

    auto* p0 = FindPathToVertex(paths, 0);
    ASSERT_NE(p0, nullptr);
    EXPECT_EQ(*p0, (ShortestPathAlgorithm<void, int, false>::Path{0}));

    auto* p1 = FindPathToVertex(paths, 1);
    ASSERT_NE(p1, nullptr);
    EXPECT_EQ(PathDistance(g, *p1), 1);

    auto* p2 = FindPathToVertex(paths, 2);
    ASSERT_NE(p2, nullptr);
    EXPECT_EQ(PathDistance(g, *p2), 3);
}

TEST(DijkstraShortestPathTest, UnreachableVertex) {
    AdjacencyList<void, int, false> g;
    g.add_vertex(0);
    g.add_vertex(1);
    g.add_vertex(2);
    g.add_edge(0, 1, 5);

    DijkstraShortestPath algo;
    auto paths = algo.FindShortestPathes(g);

    auto* p0 = FindPathToVertex(paths, 0);
    ASSERT_NE(p0, nullptr);

    auto* p1 = FindPathToVertex(paths, 1);
    ASSERT_NE(p1, nullptr);
    EXPECT_EQ(PathDistance(g, *p1), 5);

    auto* p2 = FindPathToVertex(paths, 2);
    if (p2 != nullptr) {
        EXPECT_TRUE(p2->empty());
    }
}

TEST(DijkstraShortestPathTest, IgnoresNegativeEdges) {
    AdjacencyList<void, int, false> g;
    g.add_vertex(0);
    g.add_vertex(1);
    g.add_vertex(2);
    g.add_edge(0, 1, 10);
    g.add_edge(1, 2, -5);
    g.add_edge(0, 2, 3);

    DijkstraShortestPath algo;
    auto paths = algo.FindShortestPathes(g);

    auto* p2 = FindPathToVertex(paths, 2);
    ASSERT_NE(p2, nullptr);
    EXPECT_EQ(PathDistance(g, *p2), 3);
}

