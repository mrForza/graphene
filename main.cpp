#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>

#include "graph_types.h"
#include "dijkstra.h"
#include "bellman_ford.h"
#include "bmssp.h"
#include "graph_generators.h"
#include "benchmark.h"
#include "graph_utils.h"
#include "config.h"

static std::string escape_csv(const std::string& s) {
    if (s.find(',') != std::string::npos || s.find('"') != std::string::npos || s.find('\n') != std::string::npos) {
        std::string escaped = s;
        size_t pos = 0;
        while ((pos = escaped.find('"', pos)) != std::string::npos) {
            escaped.insert(pos, "\"");
            pos += 2;
        }
        return "\"" + escaped + "\"";
    }
    return s;
}

int main(int argc, char* argv[]) {
    std::string config_file = "config.yaml";
    if (argc > 1) {
        config_file = argv[1];
    }

    Config config;
    try {
        config = parse_config(config_file);
    } catch (const std::exception& e) {
        std::cerr << "Failed to parse config: " << e.what() << "\n";
        return 1;
    }

    std::cout << "Experiment\tGenerator\tGraph\tVertices\tEdges\tAlgorithm\tAvgTime_ms\tMinTime_ms\tMaxTime_ms\tStdDev_ms\tIterations\n";

    for (size_t exp_idx = 0; exp_idx < config.experiments.size(); ++exp_idx) {
        const auto& exp = config.experiments[exp_idx];

        std::vector<Graph> graphs;
        try {
            graphs = generate_graphs_for_experiment(exp);
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << "\n";
            continue;
        }

        for (size_t g_idx = 0; g_idx < graphs.size(); ++g_idx) {
            const auto& graph = graphs[g_idx];
            long long edge_count = 0;
            for (int i = 0; i < graph.size(); ++i) {
                edge_count += static_cast<long long>(graph.adj[i].size());
            }

            std::string graph_label = graph.name.empty() ? exp.generator_type : graph.name;

            for (const auto& algo : exp.algorithms) {
                BenchmarkResult result;
                result.vertices = graph.size();
                result.iterations = 0;
                result.success = true;

                try {
                    if (algo.name == "dijkstra") {
                        result = run_benchmark(
                            graph,
                            [&graph](int s) { return dijkstra(graph, s); },
                            exp.benchmark.iterations,
                            exp.benchmark.warmup,
                            algo.start_node
                        );
                        result.algorithm_name = "dijkstra";
                    } else if (algo.name == "bellman_ford") {
                        result = run_benchmark(
                            graph,
                            [&graph](int s) { return bellman_ford(graph, s); },
                            exp.benchmark.iterations,
                            exp.benchmark.warmup,
                            algo.start_node
                        );
                        result.algorithm_name = "bellman_ford";
                    } else if (algo.name == "bmssp") {
                        auto edges = graph.edges();
                        auto solver = std::make_unique<bmssp<double>>(graph.size());
                        for (const auto& [u, v, w] : edges) {
                            solver->addEdge(u, v, w);
                        }
                        solver->prepare_graph(true);

                        result = run_benchmark(
                            graph,
                            [solver = std::move(solver)](int s) mutable {
                                auto [dist, _] = solver->execute(s);
                                return dist;
                            },
                            exp.benchmark.iterations,
                            exp.benchmark.warmup,
                            algo.start_node
                        );
                        result.algorithm_name = "bmssp";
                    } else {
                        result.success = false;
                        result.error_msg = "Unknown algorithm: " + algo.name;
                    }
                } catch (const std::exception& e) {
                    result.success = false;
                    result.error_msg = e.what();
                }

                std::cout << escape_csv(exp.name) << "\t"
                          << exp.generator_type << "\t"
                          << escape_csv(graph_label) << "\t"
                          << graph.size() << "\t"
                          << edge_count << "\t"
                          << algo.name << "\t";

                if (result.success) {
                    std::cout << std::fixed << std::setprecision(4)
                              << result.avg_time_ms << "\t"
                              << result.min_time_ms << "\t"
                              << result.max_time_ms << "\t"
                              << result.std_dev_ms << "\t"
                              << result.iterations;
                } else {
                    std::cout << "ERROR\t\t\t\t0";
                }
                std::cout << "\n";
            }
        }
    }

    return 0;
}
