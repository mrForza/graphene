#ifndef SHORTEST_PATH_H
#define SHORTEST_PATH_H

#include <cstddef>
#include <vector>

#include "../../algs/general/graph/graph.h"

namespace graph {

template<typename VertexData = void, typename EdgeWeight = int, bool Directed = false>
class ShortestPathAlgorithm {
public:
    using GraphType = Graph<VertexData, EdgeWeight, Directed>;
    using Path = std::vector<std::size_t>;
    using Paths = std::vector<Path>;

    virtual ~ShortestPathAlgorithm() = default;

    virtual Paths FindShortestPathes(const GraphType& graph) const = 0;
};

}

#endif // SHORTEST_PATH_H

