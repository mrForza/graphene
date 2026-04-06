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
    const std::vector<int> sizes = {10, 100, 1000};
    for (auto size : sizes)
    {
        graphs.emplace_back(generators::gen_path(size, true));
        std::cout << "gen_path: " << size << '\n';
        graphs.emplace_back(generators::gen_circle(size, true));
        std::cout << "gen_circle: " << size << '\n';
        graphs.emplace_back(generators::gen_tree(size, true, static_cast<int>(size * 0.2)));
        std::cout << "gen_tree 0.2: " << size << '\n';
        graphs.emplace_back(generators::gen_tree(size, true, static_cast<int>(size * 0.8)));
        std::cout << "gen_path 0.8: " << size << '\n';
        graphs.emplace_back(generators::gen_complete_k_partite(size, static_cast<int>(size * 0.1), true));
        std::cout << "gen_complete_k_partite 0.1: " << size << '\n';
        graphs.emplace_back(generators::gen_complete_k_partite(size, static_cast<int>(size * 0.3), true));
        std::cout << "gen_complete_k_partite 0.3: " << size << '\n';
        graphs.emplace_back(generators::generate_maximal_planar(size, true));
        std::cout << "generate_maximal_planar: " << size << '\n';
    }

    return graphs;
}

int main() {
    std::setlocale(LC_ALL, "");

    const auto graphs = generate_graphs();
    std::vector<BenchmarkResult> results;
    constexpr int start_node = 0;
    constexpr int iterations = 10;
    constexpr int warmup = 3;

    for (auto graph : graphs)
    {
        const auto graph_name = graph.name;
        auto res_dijkstra = run_benchmark(
            graph,
            dijkstra,
            iterations,
            warmup,
            graph,
            start_node
        );
        res_dijkstra.algorithm_name = "Dijkstra " + graph_name;
        results.push_back(res_dijkstra);
        auto res_bf = run_benchmark(
            graph,
            bellman_ford,
            iterations,
            warmup,
            graph,
            start_node
        );
        res_bf.algorithm_name = "Bellman-Ford " + graph_name;
        results.push_back(res_bf);
        BMSSPSolver bmssp(graph);
        auto res_bm = run_benchmark(
            graph,
            [&](const int source) {
                return bmssp.solve_sssp(source);
            },
            iterations,
            warmup,
            start_node
        );
        res_bm.algorithm_name = "BM-SSP " + graph_name;
        results.push_back(res_bm);
    }
    print_benchmark_report(results);
    return 0;
}
