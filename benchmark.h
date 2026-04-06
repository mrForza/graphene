#ifndef SMALLCPPPROGRAM_BENCHMARK_H
#define SMALLCPPPROGRAM_BENCHMARK_H

#include "graph_types.h"
#include <chrono>
#include <vector>
#include <string>
#include <iostream>
#include <numeric>
#include <algorithm>
#include <iomanip>
#include <cmath>
#include <functional>

struct BenchmarkResult {
    std::string algorithm_name;
    int vertices;
    int edges;
    double avg_time_ms;
    double min_time_ms;
    double max_time_ms;
    double std_dev_ms;
    int iterations;
    bool success;
    std::string error_msg;
};

template <typename AlgoFunc, typename... Args>
BenchmarkResult run_benchmark(
    const Graph& graph,
    AlgoFunc algorithm,
    int iterations,
    int warmup_runs,
    Args&&... args
) {
    BenchmarkResult result;
    result.vertices = graph.size();
    result.iterations = 0;
    result.success = true;

    long long edge_count = 0;
    for (int i = 0; i < graph.size(); ++i) {
        edge_count += static_cast<long long>(graph.adj[i].size());
    }
    result.edges = static_cast<int>(edge_count);

    std::vector<double> times_ms;
    times_ms.reserve(iterations);
    using namespace std::chrono;
    auto run_once = [&]() -> auto {
        return std::invoke(algorithm, std::forward<Args>(args)...);
    };
    for (int i = 0; i < warmup_runs; ++i) {
        try {
            auto res = run_once();
            volatile size_t dummy = 0;
            if constexpr (requires { res.empty(); }) {
                if (!res.empty()) dummy = 1;
            } else if constexpr (requires { res.begin(); }) {
                if (res.begin() != res.end()) dummy = 1;
            }
            (void)dummy;
        } catch (...) {}
    }
    for (int i = 0; i < iterations; ++i) {
        auto t_start = high_resolution_clock::now();
        try {
            auto res = run_once();
            volatile size_t dummy = 0;
            if constexpr (requires { res.empty(); }) {
                if (!res.empty()) dummy = 1;
            } else if constexpr (requires { res.begin(); }) {
                if (res.begin() != res.end()) dummy = 1;
            }
            (void)dummy;

        } catch (const std::exception& e) {
            result.success = false;
            result.error_msg = e.what();
            break;
        } catch (...) {
            result.success = false;
            result.error_msg = "Unknown exception";
            break;
        }

        auto t_end = high_resolution_clock::now();
        duration<double, std::milli> elapsed = t_end - t_start;
        times_ms.push_back(elapsed.count());
    }

    if (!result.success || times_ms.empty()) {
        result.avg_time_ms = 0.0;
        result.min_time_ms = 0.0;
        result.max_time_ms = 0.0;
        result.std_dev_ms = 0.0;
        return result;
    }

    result.iterations = static_cast<int>(times_ms.size());
    std::sort(times_ms.begin(), times_ms.end());

    double sum = std::accumulate(times_ms.begin(), times_ms.end(), 0.0);
    result.avg_time_ms = sum / times_ms.size();
    result.min_time_ms = times_ms.front();
    result.max_time_ms = times_ms.back();

    double sq_sum = 0.0;
    for (double t : times_ms) {
        sq_sum += (t - result.avg_time_ms) * (t - result.avg_time_ms);
    }
    result.std_dev_ms = std::sqrt(sq_sum / times_ms.size());

    return result;
}

inline void print_benchmark_report(const std::vector<BenchmarkResult>& results) {
    if (results.empty()) return;
    size_t max_name_len = 0;
    for (const auto& res : results) {
        if (res.algorithm_name.length() > max_name_len) {
            max_name_len = res.algorithm_name.length();
        }
    }
    const size_t padding = 3;
    const size_t name_col_width = max_name_len + padding;
    std::cout << "\n";
    std::string line(max_name_len + padding + 12, '=');
    std::cout << line << "\n";
    std::locale ru_locale("");
    try {
        std::locale::global(std::locale(""));
    } catch (...) {  }

    for (const auto& res : results) {
        if (!res.success) {
            std::cout << res.algorithm_name << " [ERROR: " << res.error_msg << "]\n";
            continue;
        }
        std::ostringstream oss;
        oss.imbue(std::locale::classic());
        oss << std::fixed << std::setprecision(4) << res.avg_time_ms;
        std::string time_str = oss.str();
        std::replace(time_str.begin(), time_str.end(), '.', ',');
        std::cout << std::left << std::setw(static_cast<int>(name_col_width)) << res.algorithm_name
                  << time_str << "\n";
    }
    std::cout << line << "\n\n";
}

#endif //SMALLCPPPROGRAM_BENCHMARK_H