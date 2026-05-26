#include <iostream>
#include <vector>
#include <locale>
#include <fstream>
#include <sstream>

#include "graph_types.h"
#include "dijkstra.h"
#include "bellman_ford.h"
#include "bmssp.h"
#include "graph_generators.h"
#include "benchmark.h"
#include "graph_utils.h"

void log_results(const std::vector<double>& distances,
                 const std::string& algorithm_name,
                 const std::string& filename = "res.log",
                 const bool append = true)
{
    std::ofstream file;
    if (append) {
        file.open(filename, std::ios::app);
    } else {
        file.open(filename, std::ios::out | std::ios::trunc);
    }
    if (!file.is_open()) {
        std::cerr << "[Error] Cannot open file " << filename << " for writing.\n";
        return;
    }

    file << "---" << algorithm_name << "---\n";
    file << std::fixed << std::setprecision(4);
    constexpr double INF = std::numeric_limits<double>::infinity();
    for (int i = 0; i < distances.size(); ++i)
    {
        if (std::isinf(distances[i])) {
            file << i << ": INF\n";
        } else {
            file << i << ": " << distances[i] << "\n";
        }
    }
    file << "\n";
    file.close();
}

std::vector<Graph> generate_graphs()
{
    std::vector<Graph> graphs;
    const std::vector<int> sizes = {1000};
    for (auto size : sizes)
    {
        graphs.emplace_back(generators::gen_path(size, true));
        std::cout << "gen_path: " << size << '\n';
        // graphs.emplace_back(generators::gen_complete_k_partite(1000, 10, true));
        // std::cout << "gen_k_partite: " << size << '\n';
        // graphs.emplace_back(generators::gen_circle(size, true));
        // std::cout << "gen_circle: " << size << '\n';
        // graphs.emplace_back(generators::gen_tree(size, true, static_cast<int>(size * 0.2)));
        // std::cout << "gen_tree 0.2: " << size << '\n';
        // graphs.emplace_back(generators::gen_tree(size, true, static_cast<int>(size * 0.8)));
        // std::cout << "gen_path 0.8: " << size << '\n';
        // graphs.emplace_back(generators::gen_complete_k_partite(size, static_cast<int>(size * 0.1), true));
        // std::cout << "gen_complete_k_partite 0.1: " << size << '\n';
        // graphs.emplace_back(generators::gen_complete_k_partite(size, static_cast<int>(size * 0.3), true));
        // std::cout << "gen_complete_k_partite 0.3: " << size << '\n';
        // graphs.emplace_back(generators::generate_maximal_planar(size, true));
        // std::cout << "generate_maximal_planar: " << size << '\n';
    }

    return graphs;
}

int main() {
    // const auto graph = generators::gen_random_graph(50, 0.1, 1,
    //     generators::CycleType::PositiveCycles, generators::ConnectivityType::StronglyConnected, true);
    // graph_utils::save_graph_as_matrix(graph, "graph.log", "0");
    // auto dijkstra_res = dijkstra(graph, 0);
    // auto edges = graph.edges();
    // bmssp<double> solver(graph.size());
    // for (const auto& [u, v, weight] : edges) {
    //     solver.addEdge(u, v, weight);
    // }
    // solver.prepare_graph(true);
    // auto [distances, _] = solver.execute(0);
    // std::cout << "bmssp:    ";
    // for (const auto distance : distances)
    // {
    //     std::cout << distance << ' ';
    // }
    // std::cout << '\n';
    // std::cout << "dijkstra: ";
    // for (const auto distance : dijkstra_res)
    // {
    //     std::cout << distance << ' ';
    // }
    //
    // std::cout << '\n';

    const auto graph = generators::gen_random_graph(200, 0.02, 1,
        generators::CycleType::PositiveCycles, generators::ConnectivityType::StronglyConnected, true);
    graph_utils::save_graph_as_matrix(graph, "graph.log", "0");
    auto dijkstra_res = dijkstra(graph, 0);

    auto edges = graph.edges();
    bmssp<double> solver(graph.size());
    for (const auto& [u, v, weight] : edges) {
        solver.addEdge(u, v, weight);
    }
    solver.prepare_graph(true);
    auto [distances, _] = solver.execute(0);
    return 0;
}
