#include "graph_generators.h"
#include <random>
#include <map>
#include <vector>
#include <set>
#include <algorithm>

namespace generators {
    constexpr double FIXED_WEIGHT_MIN = 0.0;

    constexpr double FIXED_WEIGHT_MAX = 1000.0;

    Graph gen_path(const int n, const bool is_directed) {
        if (n <= 0) return Graph(0);
        Graph graph(n);

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<double> dist(FIXED_WEIGHT_MIN, FIXED_WEIGHT_MAX);

        for (int i = 0; i < n - 1; ++i) {
            const double weight = dist(gen);
            graph.add_edge(i, i + 1, weight);
            if (!is_directed) graph.add_edge(i + 1, i, weight);
        }
        graph.name = "graph-path [n=" + std::to_string(n) + "]";
        return graph;
    }

    Graph gen_circle(const int n, const bool is_directed) {
        if (n <= 0) return Graph(0);
        Graph graph(n);
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<double> dist(FIXED_WEIGHT_MIN, FIXED_WEIGHT_MAX);

        for (int i = 0; i < n; ++i) {
            const int u = i;
            const int v = (i + 1) % n;
            const double weight = dist(gen);
            graph.add_edge(u, v, weight);
            if (!is_directed) graph.add_edge(v, u, weight);
        }
        graph.name = "graph-circle [n=" + std::to_string(n) + "]";
        return graph;
    }

    Graph gen_tree(const int n, const bool is_directed, const int branching_range) {
        if (n <= 0) return Graph(0);
        if (n == 1) return Graph(1);

        Graph graph(n);
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<double> weight_dist(FIXED_WEIGHT_MIN, FIXED_WEIGHT_MAX);
        std::vector<int> current_children_count(n, 0);
        std::vector<int> available_parents;
        available_parents.reserve(n);
        available_parents.push_back(0);

        for (int new_node = 1; new_node < n; ++new_node) {
            if (available_parents.empty()) {
                std::uniform_int_distribution<int> any_dist(0, new_node - 1);
                int parent = any_dist(gen);
                double w = weight_dist(gen);
                graph.add_edge(parent, new_node, w);
                if (!is_directed) graph.add_edge(new_node, parent, w);
                continue;
            }
            std::uniform_int_distribution<int> parent_idx_dist(0, static_cast<int>(available_parents.size()) - 1);
            const int parent_idx = parent_idx_dist(gen);
            const int parent = available_parents[parent_idx];

            const double w = weight_dist(gen);
            graph.add_edge(parent, new_node, w);
            if (!is_directed) graph.add_edge(new_node, parent, w);

            current_children_count[parent]++;
            if (current_children_count[parent] >= branching_range) {
                available_parents[parent_idx] = available_parents.back();
                available_parents.pop_back();
            }
            available_parents.push_back(new_node);
        }
        graph.name = "graph-circle [n=" + std::to_string(n) + ", branching_range=" + std::to_string(branching_range) + "]";
        return graph;
    }

    Graph gen_grid(const int rows, const int cols, const bool is_directed, const bool allow_diagonals, const bool is_toroidal) {
        if (rows <= 0 || cols <= 0) return Graph(0);

        const int n = rows * cols;
        Graph graph(n);
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<double> weight_dist(FIXED_WEIGHT_MIN, FIXED_WEIGHT_MAX);

        std::vector<std::pair<int, int>> directions = {
            {-1, 0}, {1, 0}, {0, -1}, {0, 1}
        };
        if (allow_diagonals) {
            directions.emplace_back(-1, -1);
            directions.emplace_back(-1, 1);
            directions.emplace_back(1, -1);
            directions.emplace_back(1, 1);
        }
        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                const int u = r * cols + c;
                for (const auto& [dr, dc] : directions) {
                    int nr = r + dr;
                    int nc = c + dc;
                    if (is_toroidal) {
                        nr = (nr + rows) % rows;
                        nc = (nc + cols) % cols;
                    } else {
                        if (nr < 0 || nr >= rows || nc < 0 || nc >= cols) {
                            continue;
                        }
                    }
                    const int v = nr * cols + nc;
                    if (!is_directed && u > v) {
                        continue;
                    }
                    const double w = weight_dist(gen);
                    graph.add_edge(u, v, w);
                    if (!is_directed) {
                        graph.add_edge(v, u, w);
                    }
                }
            }
        }
        return graph;
    }

    Graph gen_triangular_lattice(const int rows, const int cols, const bool is_directed) {
        if (rows <= 0 || cols <= 0) return Graph(0);

        const int n = rows * cols;
        Graph graph(n);
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<double> weight_dist(FIXED_WEIGHT_MIN, FIXED_WEIGHT_MAX);

        auto add_edge_weighted = [&](const int u, const int v) {
            if (u >= n || v >= n) return;
            const double w = weight_dist(gen);
            graph.add_edge(u, v, w);
            if (!is_directed) graph.add_edge(v, u, w);
        };

        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                const int u = r * cols + c;
                if (c + 1 < cols) {
                    add_edge_weighted(u, u + 1);
                }
                if (r + 1 < rows) {
                    if (c > 0) add_edge_weighted(u, (r + 1) * cols + (c - 1));
                    add_edge_weighted(u, (r + 1) * cols + c);
                }
            }
        }
        graph.name = "graph-triangle-lattice [rows=" + std::to_string(rows) + ", cols=" + std::to_string(cols) + "]";
        return graph;
    }

    Graph gen_square_lattice(const int rows, const int cols, const bool is_directed) {
        if (rows <= 0 || cols <= 0) return Graph(0);

        const int h = rows + 1;
        const int w = cols + 1;
        const int n = h * w;
        Graph graph(n);
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<double> weight_dist(FIXED_WEIGHT_MIN, FIXED_WEIGHT_MAX);

        auto add_edge_weighted = [&](int u, int v) {
            if (u == v) return;
            const double w = weight_dist(gen);
            graph.add_edge(u, v, w);
            if (!is_directed) graph.add_edge(v, u, w);
        };

        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                const int tl = r * w + c;
                const int tr = r * w + (c + 1);
                const int bl = (r + 1) * w + c;
                const int br = (r + 1) * w + (c + 1);
                add_edge_weighted(tl, tr);
                add_edge_weighted(tl, bl);
            }
        }
        for (int r = 0; r < h; ++r) {
            for (int c = 0; c < w; ++c) {
                const int u = r * w + c;
                if (c + 1 < w) {
                    add_edge_weighted(u, u + 1);
                }
                if (r + 1 < h) {
                    add_edge_weighted(u, u + w);
                }
            }
        }
        graph.name = "graph-square-lattice [rows=" + std::to_string(rows) + ", cols=" + std::to_string(cols) + "]";
        return graph;
    }

    Graph gen_hexagonal_lattice(const int rows, const int cols, const bool is_directed) {
        if (rows <= 0 || cols <= 0) return Graph(0);

        Graph graph(0);
        std::map<std::pair<int, int>, int> vertex_map;
        int next_id = 0;

        auto get_id = [&](int x, int y) {
            const std::pair<int, int> coord = {x, y};
            if (!vertex_map.contains(coord)) {
                vertex_map[coord] = next_id++;
            }
            return vertex_map[coord];
        };

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<double> weight_dist(FIXED_WEIGHT_MIN, FIXED_WEIGHT_MAX);
        struct EdgeTemp { int u, v; double w; };
        std::vector<EdgeTemp> edges;

        auto add_hex_edge = [&](int x1, int y1, int x2, int y2) {
            int u = get_id(x1, y1);
            int v = get_id(x2, y2);
            if (u != v) {
                double w = weight_dist(gen);
                edges.push_back({u, v, w});
            }
        };
        vertex_map.clear();
        edges.clear();
        next_id = 0;
        for (int r = 0; r <= rows; ++r) {
            for (int c = 0; c <= cols * 2; ++c) {

            }
        }
        vertex_map.clear();
        edges.clear();
        next_id = 0;

        std::map<std::pair<int,int>, double> unique_edges;

        auto add_unique_edge = [&](int x1, int y1, int x2, int y2) {
            int u = get_id(x1, y1);
            int v = get_id(x2, y2);
            if (u == v) return;
            if (u > v) std::swap(u, v);
            if (unique_edges.find({u, v}) == unique_edges.end()) {
                unique_edges[{u, v}] = weight_dist(gen);
            }
        };
        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                int base_x = c * 2;
                int base_y = r * 2;
                if (r % 2 != 0) base_x += 1;
                std::vector<std::pair<int,int>> hex_verts = {
                    {base_x, base_y},
                    {base_x + 1, base_y},
                    {base_x + 1, base_y + 1},
                    {base_x, base_y + 1},
                    {base_x - 1, base_y + 1},
                    {base_x - 1, base_y}
                };
                for (size_t i = 0; i < 6; ++i) {
                    const auto p1 = hex_verts[i];
                    const auto p2 = hex_verts[(i + 1) % 6];
                    add_unique_edge(p1.first, p1.second, p2.first, p2.second);
                }
            }
        }
        const int n = next_id;
        Graph final_graph(n);
        for (const auto& edge : unique_edges) {
            const int u = edge.first.first;
            const int v = edge.first.second;
            const double w = edge.second;
            final_graph.add_edge(u, v, w);
            if (!is_directed) {
                final_graph.add_edge(v, u, w);
            }
        }
        graph.name = "graph-hexagonal-lattice [rows=" + std::to_string(rows) + ", cols=" + std::to_string(cols) + "]";
        return final_graph;
    }

    Graph gen_k_partite(const std::vector<int>& partition_sizes, double edge_probability, const bool is_directed) {
        if (partition_sizes.empty()) return Graph(0);
        if (edge_probability < 0.0) edge_probability = 0.0;
        if (edge_probability > 1.0) edge_probability = 1.0;

        const int k = static_cast<int>(partition_sizes.size());
        int n = 0;
        for (const int size : partition_sizes) {
            if (size < 0) return Graph(0);
            n += size;
        }

        if (n == 0) return Graph(0);

        Graph graph(n);
        std::random_device rd;
        std::mt19937 gen(rd());;
        std::uniform_real_distribution<double> weight_dist(FIXED_WEIGHT_MIN, FIXED_WEIGHT_MAX);
        std::uniform_real_distribution<double> prob_dist(0.0, 1.0);

        std::vector<int> boundaries(k + 1, 0);
        for (int i = 0; i < k; ++i) {
            boundaries[i + 1] = boundaries[i] + partition_sizes[i];
        }
        for (int i = 0; i < k; ++i) {
            for (int j = i + 1; j < k; ++j) {
                const int start_u = boundaries[i];
                const int end_u = boundaries[i + 1];
                const int start_v = boundaries[j];
                const int end_v = boundaries[j + 1];
                for (int u = start_u; u < end_u; ++u) {
                    for (int v = start_v; v < end_v; ++v) {
                        if (prob_dist(gen) <= edge_probability) {
                            const double w = weight_dist(gen);
                            graph.add_edge(u, v, w);
                            if (!is_directed) {
                                graph.add_edge(v, u, w);
                            }
                        }
                    }
                }
            }
        }
        return graph;
    }

    Graph gen_complete_k_partite(const int n, int k, const bool is_directed) {
        if (k <= 0 || n <= 0) return Graph(0);
        if (k > n) k = n;
        std::vector<int> sizes(k, n / k);
        const int remainder = n % k;
        for (int i = 0; i < remainder; ++i) {
            sizes[i]++;
        }
        auto graph = gen_k_partite(sizes, 1.0, is_directed);
        graph.name = "graph-k-partite [n=" + std::to_string(n) + ", k=" + std::to_string(k) + "]";
        return graph;
    }

    struct Face {
        int u, v, w;
        Face(const int a, const int b, const int c) : u(a), v(b), w(c) {}
    };

    Graph generate_maximal_planar(const int n, const bool is_directed) {
        if (n < 3) {
            return gen_path(n, is_directed);
        }

        Graph graph(n);
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<double> weight_dist(FIXED_WEIGHT_MIN, FIXED_WEIGHT_MAX);
        std::uniform_int_distribution<int> face_dist(0, 0);
        std::vector<Face> faces;

        faces.emplace_back(0, 1, 2);
        auto add_edge_weighted = [&](const int u, const int v) {
            const double w = weight_dist(gen);
            graph.add_edge(u, v, w);
            if (!is_directed) {
                graph.add_edge(v, u, w);
            }
        };
        add_edge_weighted(0, 1);
        add_edge_weighted(1, 2);
        add_edge_weighted(2, 0);
        for (int new_node = 3; new_node < n; ++new_node) {
            if (faces.empty()) break;
            std::uniform_int_distribution<int> dist(0, static_cast<int>(faces.size()) - 1);
            int idx = dist(gen);
            Face f = faces[idx];

            faces[idx] = faces.back();
            faces.pop_back();

            add_edge_weighted(new_node, f.u);
            add_edge_weighted(new_node, f.v);
            add_edge_weighted(new_node, f.w);

            faces.emplace_back(new_node, f.u, f.v);
            faces.emplace_back(new_node, f.v, f.w);
            faces.emplace_back(new_node, f.w, f.u);
        }
        graph.name = "graph-planar [n=" + std::to_string(n) + "]";
        return graph;
    }

    Graph gen_planar(const int n, const double density, const bool is_directed) {
        if (density >= 1.0) {
            return generate_maximal_planar(n, is_directed);
        }
        Graph g = generate_maximal_planar(n, true);
        int current_edges = 0;
        for (int i = 0; i < n; ++i) {
            current_edges += static_cast<int>(g.adj[i].size());
        }
        struct EdgePair { int u, v; double w; };
        std::vector<EdgePair> all_edges;

        for (int u = 0; u < n; ++u) {
            for (const auto& edge : g.adj[u]) {
                const int v = edge.to;
                if (u < v) {
                    all_edges.push_back({u, v, edge.weight});
                }
            }
        }
        const int min_edges = n - 1;
        const int max_edges = static_cast<int>(all_edges.size());

        if (max_edges <= min_edges) {
             Graph final_g(n);
             for(const auto& e : all_edges) {
                 final_g.add_edge(e.u, e.v, e.w);
                 if(!is_directed) final_g.add_edge(e.v, e.u, e.w);
             }
             return final_g;
        }

        int target_edges = static_cast<int>(min_edges + (max_edges - min_edges) * density);

        std::vector<bool> visited(n, false);
        std::vector<EdgePair> tree_edges;
        std::vector<EdgePair> extra_edges;

        std::vector<std::vector<std::pair<int, double>>> temp_adj(n);
        for(const auto& e : all_edges) {
            temp_adj[e.u].emplace_back(e.v, e.w);
            temp_adj[e.v].emplace_back(e.u, e.w);
        }

        std::vector<int> stack;
        stack.push_back(0);
        visited[0] = true;

        while(!stack.empty()) {
            int u = stack.back();
            stack.pop_back();
            for(auto& neighbor : temp_adj[u]) {
                int v = neighbor.first;
                const double w = neighbor.second;
                if (!visited[v]) {
                    visited[v] = true;
                    stack.push_back(v);
                    const int u_min = (u < v) ? u : v;
                    const int v_max = (u < v) ? v : u;
                    tree_edges.push_back({u_min, v_max, w});
                } else {
                    bool exists = false;
                    const int u_min = (u < v) ? u : v;
                    const int v_max = (u < v) ? v : u;

                    for(const auto& te : tree_edges) if(te.u == u_min && te.v == v_max) { exists = true; break; }
                    if(!exists) {
                         for(const auto& ee : extra_edges) if(ee.u == u_min && ee.v == v_max) { exists = true; break; }
                    }

                    if (!exists) {
                        extra_edges.push_back({u_min, v_max, w});
                    }
                }
            }
        }

        std::random_device rd;
        std::shuffle(extra_edges.begin(), extra_edges.end(), std::mt19937(rd() + 1)); // Другой сид для шаффла


        std::vector<EdgePair> final_edges = tree_edges;
        int needed_from_extra = target_edges - static_cast<int>(tree_edges.size());
        if (needed_from_extra > 0) {
            const int count = std::min(needed_from_extra, static_cast<int>(extra_edges.size()));
            final_edges.insert(final_edges.end(), extra_edges.begin(), extra_edges.begin() + count);
        }

        Graph result(n);
        for (const auto& e : final_edges) {
            result.add_edge(e.u, e.v, e.w);
            if (!is_directed) {
                result.add_edge(e.v, e.u, e.w);
            }
        }
        return result;
    }

    Graph gen_chordal(int n, int max_clique_size, bool is_directed) {
        if (n <= 0) return Graph(0);
        if (max_clique_size < 1) max_clique_size = 1;
        if (max_clique_size > n) max_clique_size = n;

        Graph graph(n);
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<double> weight_dist(FIXED_WEIGHT_MIN, FIXED_WEIGHT_MAX);
        std::vector<int> current_clique;

        auto add_edge_weighted = [&](int u, int v) {
            if (u == v) return;
            double w = weight_dist(gen);
            graph.add_edge(u, v, w);
            if (!is_directed) {
                graph.add_edge(v, u, w);
            }
        };
        current_clique.push_back(0);

        for (int i = 1; i < n; ++i) {
            int max_neighbors = std::min(static_cast<int>(current_clique.size()), max_clique_size - 1);
            int min_neighbors = 1;
            if (max_neighbors < min_neighbors) min_neighbors = 0; // Если клика пустая (невозможно)

            std::uniform_int_distribution<int> count_dist(min_neighbors, max_neighbors);
            int k = count_dist(gen);

            std::vector<int> neighbors;
            neighbors.reserve(k);

            std::shuffle(current_clique.begin(), current_clique.end(), gen);
            for (int j = 0; j < k; ++j) {
                neighbors.push_back(current_clique[j]);
                add_edge_weighted(i, current_clique[j]);
            }

            current_clique = neighbors;
            current_clique.push_back(i);
        }
        graph.name = "graph-chordal [n=" + std::to_string(n) + ", max_clique_size=" + std::to_string(max_clique_size) + "]";
        return graph;
    }

    struct TempEdge {
        int u, v;
        double w;
    };

    struct EdgeData {
        int u, v;
        double w;
    };

    Graph gen_random_graph(
        int n,
        double density,
        int num_components,
        CycleType cycle_type,
        ConnectivityType connectivity_type,
        bool is_directed,
        int target_cycle_count_approx
    ) {
        if (n <= 0) return Graph(0);
        if (num_components < 1) num_components = 1;
        if (num_components > n) num_components = n;
        if (density < 0.0) density = 0.0;
        if (density > 1.0) density = 1.0;

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<double> weight_dist(FIXED_WEIGHT_MIN, FIXED_WEIGHT_MAX);
        std::uniform_real_distribution<double> pos_weight_dist(0.0, 1000.0);
        std::vector<int> vertices(n);
        std::iota(vertices.begin(), vertices.end(), 0);
        std::shuffle(vertices.begin(), vertices.end(), gen);

        std::vector<std::vector<int>> components(num_components);
        for (int i = 0; i < n; ++i) {
            components[i % num_components].push_back(vertices[i]);
        }

        std::vector<EdgeData> final_edges;
        long long estimated_edges = static_cast<long long>(n) * (n - 1) * density;
        if (!is_directed) estimated_edges /= 2;
        final_edges.reserve(static_cast<size_t>(estimated_edges) + n);

        auto get_weight = [&](bool force_positive) {
            if (force_positive) return pos_weight_dist(gen);
            return weight_dist(gen);
        };

        for (const auto& comp : components) {
            int k = static_cast<int>(comp.size());
            if (k == 0) continue;
            std::vector<int> sorted_comp = comp;
            std::sort(sorted_comp.begin(), sorted_comp.end());
            std::vector<std::pair<int, int>> all_possible_edges;
            long long pool_size = is_directed ?
                (static_cast<long long>(k) * (k - 1)) :
                (static_cast<long long>(k) * (k - 1) / 2);

            // Защита от выделения гигабайтов памяти, если компонента огромная, а плотность маленькая
            // Если пул слишком большой (> 50 млн ребер), лучше использовать старый метод с ограничением попыток
            // Но для N=3000 и одной компоненты пул ~4.5 млн, что легко влезает в RAM.
            if (pool_size > 50000000) { }

            all_possible_edges.reserve(static_cast<size_t>(pool_size));

            for (int i = 0; i < k; ++i) {
                int start_j = (is_directed || cycle_type == CycleType::Acyclic) ? 0 : (i + 1);
                int end_j = k;

                for (int j = start_j; j < end_j; ++j) {
                    if (i == j) continue;
                    if (is_directed && cycle_type == CycleType::Acyclic) {
                        if (i >= j) continue;
                    }
                    all_possible_edges.emplace_back(sorted_comp[i], sorted_comp[j]);
                }
            }

            long long max_edges_in_comp = static_cast<long long>(all_possible_edges.size());
            long long target_edges_in_comp = static_cast<long long>(max_edges_in_comp * density);
            long long min_edges_needed = (k > 1) ? (k - 1) : 0;
            if (is_directed && connectivity_type == ConnectivityType::StronglyConnected) {
                min_edges_needed = k;
            }

            if (target_edges_in_comp < min_edges_needed) {
                target_edges_in_comp = min_edges_needed;
            }
            if (target_edges_in_comp > max_edges_in_comp) {
                target_edges_in_comp = max_edges_in_comp;
            }

            std::vector<std::pair<int, int>> selected_edges;
            selected_edges.reserve(static_cast<size_t>(target_edges_in_comp));

            if (target_edges_in_comp == max_edges_in_comp) {
                selected_edges = std::move(all_possible_edges);
            } else {
                for (long long i = 0; i < target_edges_in_comp; ++i) {
                    std::uniform_int_distribution<size_t> dist(i, all_possible_edges.size() - 1);
                    size_t idx = dist(gen);
                    std::swap(all_possible_edges[i], all_possible_edges[idx]);
                    selected_edges.push_back(all_possible_edges[i]);
                }
            }
            bool force_positive = (cycle_type == CycleType::PositiveCycles);

            for (const auto& edge : selected_edges) {
                double w = get_weight(force_positive);
                final_edges.push_back({edge.first, edge.second, w});
                if (!is_directed) {
                    final_edges.push_back({edge.second, edge.first, w});
                }
            }
        }

        if (cycle_type == CycleType::NegativeCycles && !final_edges.empty()) {
            if (!final_edges.empty()) {
                final_edges[0].w = -2000.0;
                if (!is_directed && final_edges.size() > 1) final_edges[1].w = -2000.0;
            }
        }

        Graph graph(n);
        for (const auto& e : final_edges) {
            graph.add_edge(e.u, e.v, e.w);
        }

        graph.name = "graph-chordal [n=" + std::to_string(n) + ", density=" + std::to_string(density) +
            ", num_components=" + std::to_string(num_components) + ", target_cycle_count_approx=" + std::to_string(target_cycle_count_approx) + "]";
        return graph;
    }
} // namespace generators