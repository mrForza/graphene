#include "config.h"
#include "graph_generators.h"
#include <stdexcept>
#include <iostream>

static YAML::Node merge_nodes(YAML::Node base, const YAML::Node& overlay) {
    if (!overlay) return base;
    if (!base) base = YAML::Node(YAML::NodeType::Map);
    if (!base.IsMap() || !overlay.IsMap()) return overlay;
    for (auto it = overlay.begin(); it != overlay.end(); ++it) {
        base[it->first.as<std::string>()] = YAML::Clone(it->second);
    }
    return base;
}

static std::vector<YAML::Node> expand_sweep_params(const YAML::Node& fixed_params, const YAML::Node& sweep) {
    std::string mode = "product";
    std::vector<std::string> sweep_keys;
    std::vector<std::vector<YAML::Node>> sweep_values;

    if (sweep && sweep.IsMap()) {
        if (sweep["mode"]) {
            mode = sweep["mode"].as<std::string>();
        }
        for (auto it = sweep.begin(); it != sweep.end(); ++it) {
            auto key = it->first.as<std::string>();
            if (key == "mode") continue;
            if (it->second.IsSequence()) {
                sweep_keys.push_back(key);
                std::vector<YAML::Node> vals;
                for (auto v : it->second) {
                    vals.push_back(YAML::Clone(v));
                }
                sweep_values.push_back(vals);
            }
        }
    }

    if (sweep_keys.empty()) {
        YAML::Node merged = YAML::Clone(fixed_params);
        return {merged};
    }

    std::vector<YAML::Node> result;

    if (mode == "zip") {
        size_t len = sweep_values[0].size();
        for (size_t j = 1; j < sweep_values.size(); ++j) {
            if (sweep_values[j].size() != len) {
                throw std::runtime_error("All sweep lists must have the same length in zip mode");
            }
        }
        for (size_t i = 0; i < len; ++i) {
            YAML::Node merged = YAML::Clone(fixed_params);
            for (size_t j = 0; j < sweep_keys.size(); ++j) {
                merged[sweep_keys[j]] = YAML::Clone(sweep_values[j][i]);
            }
            result.push_back(merged);
        }
    } else {
        std::vector<size_t> indices(sweep_keys.size(), 0);
        while (true) {
            YAML::Node merged = YAML::Clone(fixed_params);
            for (size_t i = 0; i < sweep_keys.size(); ++i) {
                merged[sweep_keys[i]] = YAML::Clone(sweep_values[i][indices[i]]);
            }
            result.push_back(merged);

            size_t pos = 0;
            while (pos < indices.size()) {
                indices[pos]++;
                if (indices[pos] < sweep_values[pos].size()) break;
                indices[pos] = 0;
                pos++;
            }
            if (pos == indices.size()) break;
        }
    }

    return result;
}

static generators::CycleType parse_cycle_type(const std::string& s) {
    if (s == "Acyclic") return generators::CycleType::Acyclic;
    if (s == "PositiveCycles") return generators::CycleType::PositiveCycles;
    if (s == "NegativeCycles") return generators::CycleType::NegativeCycles;
    throw std::runtime_error("Unknown cycle_type: " + s);
}

static generators::ConnectivityType parse_connectivity_type(const std::string& s) {
    if (s == "WeaklyConnected") return generators::ConnectivityType::WeaklyConnected;
    if (s == "StronglyConnected") return generators::ConnectivityType::StronglyConnected;
    throw std::runtime_error("Unknown connectivity_type: " + s);
}

static std::vector<int> parse_int_list(const YAML::Node& node) {
    std::vector<int> result;
    if (node.IsSequence()) {
        for (auto v : node) result.push_back(v.as<int>());
    } else {
        result.push_back(node.as<int>());
    }
    return result;
}

static Graph create_graph(const std::string& type, const YAML::Node& params) {
    bool directed = params["directed"] ? params["directed"].as<bool>() : true;

    if (type == "path") {
        int n = params["n"].as<int>();
        auto g = generators::gen_path(n, directed);
        return g;
    }
    if (type == "circle") {
        int n = params["n"].as<int>();
        return generators::gen_circle(n, directed);
    }
    if (type == "tree") {
        int n = params["n"].as<int>();
        int br = params["branching_range"] ? params["branching_range"].as<int>() : 3;
        return generators::gen_tree(n, directed, br);
    }
    if (type == "grid") {
        int rows = params["rows"].as<int>();
        int cols = params["cols"].as<int>();
        bool diag = params["allow_diagonals"] ? params["allow_diagonals"].as<bool>() : false;
        bool tor = params["is_toroidal"] ? params["is_toroidal"].as<bool>() : false;
        return generators::gen_grid(rows, cols, directed, diag, tor);
    }
    if (type == "triangular_lattice") {
        int rows = params["rows"].as<int>();
        int cols = params["cols"].as<int>();
        return generators::gen_triangular_lattice(rows, cols, directed);
    }
    if (type == "square_lattice") {
        int rows = params["rows"].as<int>();
        int cols = params["cols"].as<int>();
        return generators::gen_square_lattice(rows, cols, directed);
    }
    if (type == "hexagonal_lattice") {
        int rows = params["rows"].as<int>();
        int cols = params["cols"].as<int>();
        return generators::gen_hexagonal_lattice(rows, cols, directed);
    }
    if (type == "k_partite") {
        auto sizes = parse_int_list(params["partition_sizes"]);
        double prob = params["edge_probability"] ? params["edge_probability"].as<double>() : 1.0;
        return generators::gen_k_partite(sizes, prob, directed);
    }
    if (type == "complete_k_partite") {
        int n = params["n"].as<int>();
        int k = params["k"].as<int>();
        return generators::gen_complete_k_partite(n, k, directed);
    }
    if (type == "planar") {
        int n = params["n"].as<int>();
        double density = params["density"] ? params["density"].as<double>() : 1.0;
        return generators::gen_planar(n, density, directed);
    }
    if (type == "maximal_planar") {
        int n = params["n"].as<int>();
        return generators::generate_maximal_planar(n, directed);
    }
    if (type == "chordal") {
        int n = params["n"].as<int>();
        int mcs = params["max_clique_size"] ? params["max_clique_size"].as<int>() : 5;
        return generators::gen_chordal(n, mcs, directed);
    }
    if (type == "random") {
        int n = params["n"].as<int>();
        double density = params["density"] ? params["density"].as<double>() : 0.1;
        int nc = params["num_components"] ? params["num_components"].as<int>() : 1;
        auto ct = params["cycle_type"] ? parse_cycle_type(params["cycle_type"].as<std::string>()) : generators::CycleType::PositiveCycles;
        auto cnt = params["connectivity_type"] ? parse_connectivity_type(params["connectivity_type"].as<std::string>()) : generators::ConnectivityType::StronglyConnected;
        int tca = params["target_cycle_count_approx"] ? params["target_cycle_count_approx"].as<int>() : -1;
        return generators::gen_random_graph(n, density, nc, ct, cnt, directed, tca);
    }

    throw std::runtime_error("Unknown generator type: " + type);
}

Config parse_config(const std::string& filename) {
    YAML::Node root = YAML::LoadFile(filename);
    Config config;

    if (!root["experiments"] || !root["experiments"].IsSequence()) {
        throw std::runtime_error("Config must contain an 'experiments' sequence");
    }

    for (const auto& exp_node : root["experiments"]) {
        ExperimentConfig exp;
        exp.name = exp_node["name"] ? exp_node["name"].as<std::string>() : "unnamed";

        auto gen = exp_node["generator"];
        exp.generator_type = gen["type"].as<std::string>();
        exp.params = gen["params"] ? gen["params"] : YAML::Node();
        exp.sweep = gen["sweep"] ? gen["sweep"] : YAML::Node();

        for (const auto& algo_node : exp_node["algorithms"]) {
            AlgorithmConfig algo;
            algo.name = algo_node["name"].as<std::string>();
            algo.start_node = algo_node["start_node"] ? algo_node["start_node"].as<int>() : 0;
            exp.algorithms.push_back(algo);
        }

        if (exp_node["benchmark"]) {
            exp.benchmark.iterations = exp_node["benchmark"]["iterations"] ? exp_node["benchmark"]["iterations"].as<int>() : 5;
            exp.benchmark.warmup = exp_node["benchmark"]["warmup"] ? exp_node["benchmark"]["warmup"].as<int>() : 2;
        }

        config.experiments.push_back(exp);
    }

    return config;
}

std::vector<Graph> generate_graphs_for_experiment(const ExperimentConfig& exp) {
    auto param_sets = expand_sweep_params(exp.params, exp.sweep);
    std::vector<Graph> graphs;
    graphs.reserve(param_sets.size());
    for (const auto& params : param_sets) {
        graphs.push_back(create_graph(exp.generator_type, params));
    }
    return graphs;
}
