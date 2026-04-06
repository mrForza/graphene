#include "graph_utils.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <limits>

namespace graph_utils {
    const double INF_WEIGHT = std::numeric_limits<double>::infinity();

    std::vector<std::vector<double>> to_adjacency_matrix(const Graph& graph) {
        const int n = graph.size();
        std::vector<std::vector<double>> matrix(n, std::vector<double>(n, INF_WEIGHT));

        for (int i = 0; i < n; ++i) {
            matrix[i][i] = 0.0;
        }
        for (int u = 0; u < n; ++u) {
            for (const auto& edge : graph.adj[u]) {
                const int v = edge.to;
                const double w = edge.weight;
                matrix[u][v] = w;
            }
        }
        return matrix;
    }

    std::string matrix_to_string(const std::vector<std::vector<double>>& matrix, const std::string& infinity_symbol) {
        std::ostringstream oss;
        if (matrix.empty()) return "";
        const int n = static_cast<int>(matrix.size());
        // oss << n << "\n";
        oss << std::fixed << std::setprecision(2);

        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                if (std::isinf(matrix[i][j])) {
                    oss << infinity_symbol;
                } else {
                    oss << matrix[i][j];
                }
                if (j < n - 1) {
                    oss << ", ";
                }
            }
            oss << "\n";
        }

        return oss.str();
    }

    bool save_matrix_to_file(const std::vector<std::vector<double>>& matrix, const std::string& filename, const std::string& infinity_symbol) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        file << matrix_to_string(matrix, infinity_symbol);
        file.close();
        return !file.fail();
    }

    bool save_graph_as_matrix(const Graph& graph, const std::string& filename, const std::string& infinity_symbol) {
        const auto matrix = to_adjacency_matrix(graph);
        return save_matrix_to_file(matrix, filename, infinity_symbol);
    }
} // namespace graph_utils