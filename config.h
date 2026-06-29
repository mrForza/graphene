#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <vector>
#include <yaml-cpp/yaml.h>
#include "graph_types.h"

struct AlgorithmConfig {
    std::string name;
    int start_node = 0;
};

struct BenchmarkConfig {
    int iterations = 5;
    int warmup = 2;
};

struct ExperimentConfig {
    std::string name;
    std::string generator_type;
    YAML::Node params;
    YAML::Node sweep;
    std::vector<AlgorithmConfig> algorithms;
    BenchmarkConfig benchmark;
};

struct Config {
    std::vector<ExperimentConfig> experiments;
};

Config parse_config(const std::string& filename);

std::vector<Graph> generate_graphs_for_experiment(const ExperimentConfig& exp);

#endif
