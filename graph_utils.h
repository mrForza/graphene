#ifndef SMALLCPPPROGRAM_GRAPH_UTILS_H
#define SMALLCPPPROGRAM_GRAPH_UTILS_H

#include <vector>
#include <string>
#include <map>

#include "graph_types.h"

namespace graph_utils {
    extern const double INF_WEIGHT;

    std::vector<std::vector<double>> to_adjacency_matrix(const Graph& graph);

    std::string matrix_to_string(
        const std::vector<std::vector<double>>& matrix,
        const std::string& infinity_symbol = "0");

    bool save_matrix_to_file(
        const std::vector<std::vector<double>>& matrix,
        const std::string& filename,
        const std::string& infinity_symbol = "0");

    bool save_graph_as_matrix(
        const Graph& graph,
        const std::string& filename,
        const std::string& infinity_symbol = "0");
} // namespace graph_utils

#endif //SMALLCPPPROGRAM_GRAPH_UTILS_H