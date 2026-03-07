#include <gtest/gtest.h>
#include <numeric>

#include "../general/graph/edge_list.hpp"
#include "../interfaces/shortest_path.h"
#include "bellman_ford.cpp"

using namespace graph;

namespace {

int PathDistance(const Graph<void, int, true>& graph,
                 const ShortestPathAlgorithm<void, int, true>::Path& path) {
    int sum = 0;
    for (std::size_t i = 0; i + 1 < path.size(); ++i) {
        sum += graph.get_edge_weight(path[i], path[i + 1]);
    }
    return sum;
}

const ShortestPathAlgorithm<void, int, true>::Path*
FindPathToVertex(const ShortestPathAlgorithm<void, int, true>::Paths& paths,
                 std::size_t target) {
    for (const auto& p : paths) {
        if (!p.empty() && p.back() == target) {
            return &p;
        }
    }
    return nullptr;
}

} // namespace

TEST(BellmanFordShortestPathTest, SimpleDirectedGraph) {
    EdgeList<void, int, true> g;
    g.add_vertex(0);
    g.add_vertex(1);
    g.add_vertex(2);
    g.add_edge(0, 1, 4);
    g.add_edge(1, 2, 1);
    g.add_edge(0, 2, 10);

    BellmanFordShortestPath algo;
    auto paths = algo.FindShortestPathes(g);

    auto* p2 = FindPathToVertex(paths, 2);
    ASSERT_NE(p2, nullptr);
    EXPECT_EQ(PathDistance(g, *p2), 5);
}

TEST(BellmanFordShortestPathTest, HandlesNegativeWeights) {
    EdgeList<void, int, true> g;
    g.add_vertex(0);
    g.add_vertex(1);
    g.add_vertex(2);
    g.add_edge(0, 1, 2);
    g.add_edge(1, 2, -5);
    g.add_edge(0, 2, 10);

    BellmanFordShortestPath algo;
    auto paths = algo.FindShortestPathes(g);

    auto* p2 = FindPathToVertex(paths, 2);
    ASSERT_NE(p2, nullptr);
    EXPECT_EQ(PathDistance(g, *p2), -3);
}

TEST(BellmanFordShortestPathTest, UnreachableVertex) {
    EdgeList<void, int, true> g;
    g.add_vertex(0);
    g.add_vertex(1);
    g.add_vertex(2);
    g.add_edge(0, 1, 1);

    BellmanFordShortestPath algo;
    auto paths = algo.FindShortestPathes(g);

    auto* p1 = FindPathToVertex(paths, 1);
    ASSERT_NE(p1, nullptr);
    EXPECT_EQ(PathDistance(g, *p1), 1);

    auto* p2 = FindPathToVertex(paths, 2);
    if (p2 != nullptr) {
        EXPECT_TRUE(p2->empty());
    }
}

