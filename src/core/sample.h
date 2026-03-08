#ifndef CORE_SAMPLE_H
#define CORE_SAMPLE_H

#include <string>
#include <vector>
#include <memory>

#include "../../algs/general/graph/graph.h"

namespace core {

class Sample {
public:
    using GraphType = graph::Graph<>;
    using GraphPtr = std::shared_ptr<GraphType>;
    using GraphContainer = std::vector<GraphPtr>;
    using iterator = GraphContainer::iterator;
    using const_iterator = GraphContainer::const_iterator;

    Sample() = default;

    Sample(std::string id,
           std::string name,
           GraphContainer graphs = {})
        : id_(std::move(id)),
          name_(std::move(name)),
          graphs_(std::move(graphs)) {}

    const GraphContainer& graphs() const { return graphs_; }
    GraphContainer& graphs() { return graphs_; }

    iterator begin() { return graphs_.begin(); }
    iterator end() { return graphs_.end(); }

    const_iterator begin() const { return graphs_.cbegin(); }
    const_iterator end() const { return graphs_.cend(); }

    const_iterator cbegin() const { return graphs_.cbegin(); }
    const_iterator cend() const { return graphs_.cend(); }

private:
    std::string name_;
    GraphContainer graphs_;
};

} // namespace core

#endif // CORE_SAMPLE_H

