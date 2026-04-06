#ifndef SMALLCPPPROGRAM_GRAPH_GENERATORS_H
#define SMALLCPPPROGRAM_GRAPH_GENERATORS_H

#include "graph_types.h"

namespace generators {
    enum class CycleType {
        Acyclic,
        PositiveCycles,
        NegativeCycles
    };

    enum class ConnectivityType {
        WeaklyConnected,
        StronglyConnected
    };

    Graph gen_path(int n, bool is_directed);

    Graph gen_circle(int n, bool is_directed);

    Graph gen_tree(int n, bool is_directed, int branching_range);

    Graph gen_grid(int rows, int cols, bool is_directed, bool allow_diagonals = false, bool is_toroidal = false);

    Graph gen_triangular_lattice(int rows, int cols, bool is_directed);

    Graph gen_square_lattice(int rows, int cols, bool is_directed);

    Graph gen_hexagonal_lattice(int rows, int cols, bool is_directed);

    Graph gen_k_partite(const std::vector<int>& partition_sizes, double edge_probability, bool is_directed);

    Graph gen_complete_k_partite(int n, int k, bool is_directed);

    Graph gen_planar(int n, double density, bool is_directed);

    Graph generate_maximal_planar(const int n, const bool is_directed);

    Graph gen_chordal(int n, int max_clique_size, bool is_directed);

    Graph gen_random_graph(
        int n,
        double density,
        int num_components,
        CycleType cycle_type,
        ConnectivityType connectivity_type,
        bool is_directed,
        int target_cycle_count_approx = -1
    );
}

#endif //SMALLCPPPROGRAM_GRAPH_GENERATORS_H