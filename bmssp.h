#ifndef SMALLCPPPROGRAM_BMSSP_H
#define SMALLCPPPROGRAM_BMSSP_H

#include "graph_types.h"
#include <vector>
#include <queue>
#include <set>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <cmath>
#include <limits>
#include <memory>
#include <functional>

constexpr double INF = std::numeric_limits<double>::infinity();
constexpr double EPS = 1e-9;

class BlockLinkedList {
public:
    BlockLinkedList(const int M_, const double B_) : M(M_), B(B_) {}

    void insert(const int key, const double value) {
        if (!data.contains(key) || value < data[key]) {
            data[key] = value;
        }
    }

    void batch_prepend(const std::vector<std::pair<int, double>>& L) {
        for (const auto& [key, value] : L) {
            if (!data.contains(key) || value < data[key]) {
                data[key] = value;
            }
        }
    }

    std::pair<std::vector<int>, double> pull() {
        if (data.empty()) {
            return {{}, B};
        }

        std::vector<std::pair<double, int>> sorted_items;
        for (const auto& [key, value] : data) {
            sorted_items.emplace_back(value, key);
        }
        std::sort(sorted_items.begin(), sorted_items.end());

        std::vector<int> result;
        double x = B;

        const int count = std::min(M, static_cast<int>(sorted_items.size()));
        for (int i = 0; i < count; i++) {
            result.push_back(sorted_items[i].second);
            data.erase(sorted_items[i].second);
        }

        if (!data.empty()) {
            x = data.begin()->second;
        }

        return {result, x};
    }

    [[nodiscard]] bool is_empty() const {
        return data.empty();
    }
private:
    int M;
    double B;
    std::map<int, double> data;
};

class BMSSPSolver {
public:
    explicit BMSSPSolver(Graph& g) : graph(g), db(g.n, INF), pred(g.n, -1), n(g.n) {
        k = std::max(1, static_cast<int>(std::pow(n, 1.0 / 3.0)));
        t = std::max(1, static_cast<int>(std::pow(n, 2.0 / 3.0)));
    }

    std::pair<double, std::set<int>> bmssp(int level, double B, const std::set<int>& S) {
        if (level == 0) {
            return base_case(B, S);
        }

        auto [P, W] = find_pivots(B, S);

        if (P.empty()) {
            std::set<int> U;
            for (int v : W) {
                if (db[v] < B) {
                    U.insert(v);
                }
            }
            return {B, U};
        }

        int M = std::max(1, 1 << ((level - 1) * t));
        BlockLinkedList D(M, B);

        for (int x : P) {
            D.insert(x, db[x]);
        }

        int i = 0;
        double B_prime_prev = INF;
        for (int x : P) {
            B_prime_prev = std::min(B_prime_prev, db[x]);
        }

        std::set<int> U;
        int threshold = k * (1 << (level * t));

        int max_iterations = 1000;

        while ((int)U.size() < threshold && !D.is_empty() && i < max_iterations) {
            i++;

            auto [S_i_vec, B_i] = D.pull();
            std::set<int> S_i(S_i_vec.begin(), S_i_vec.end());

            if (S_i.empty()) break;

            auto [B_prime_i, U_i] = bmssp(level - 1, B_i, S_i);
            U.insert(U_i.begin(), U_i.end());

            std::vector<std::pair<int, double>> K;
            for (int u : U_i) {
                for (const Edge& e : graph.adj[u]) {
                    if (relax_edge(u, e.to, e.weight)) {
                        double new_val = db[u] + e.weight;
                        if (B_i <= new_val && new_val < B) {
                            D.insert(e.to, new_val);
                        } else if (B_prime_i <= new_val && new_val < B_i) {
                            K.emplace_back(e.to, new_val);
                        }
                    }
                }
            }

            for (int x : S_i) {
                if (B_prime_i <= db[x] && db[x] < B_i) {
                    K.emplace_back(x, db[x]);
                }
            }
            D.batch_prepend(K);

            if (D.is_empty()) {
                B_prime_prev = B;
                break;
            }

            if (static_cast<int>(U.size()) >= threshold) {
                B_prime_prev = B_prime_i;
                break;
            }
        }

        for (int v : W) {
            if (db[v] < B_prime_prev) {
                U.insert(v);
            }
        }

        return {B_prime_prev, U};
    }

    std::vector<double> solve_sssp(int source) {
        db[source] = 0.0;

        int max_level = 1;
        if (n > 10) {
            max_level = std::max(1, std::min(3, static_cast<int>(std::ceil(std::log2(n) / t))));
        }

        auto [B_prime, U] = bmssp(max_level, INF, {source});

        std::map<int, double> distances;
        std::vector<double> res;
        for (int v = 0; v < n; v++) {
            if (db[v] < INF) {
                distances[v] = db[v];
                res.push_back(db[v]);
            } else
            {
                distances[v] = std::numeric_limits<double>::infinity();
            }
        }

        return res;
    }

    [[nodiscard]] int get_k() const { return k; }
    [[nodiscard]] int get_t() const { return t; }

private:
    Graph& graph;
    std::vector<double> db;
    std::vector<int> pred;
    int n;
    int k;
    int t;

    bool relax_edge(const int u, const int v, const double weight) {
        if (db[u] + weight <= db[v] + EPS) {
            db[v] = db[u] + weight;
            pred[v] = u;
            return true;
        }
        return false;
    }

    std::pair<std::set<int>, std::set<int>> find_pivots(const double B, const std::set<int>& S) {
        std::set<int> W = S;
        std::set<int> W_i_minus_1 = S;

        for (int i = 0; i < k; i++) {
            std::set<int> W_i;
            for (const int u : W_i_minus_1) {
                for (const Edge& e : graph.adj[u]) {
                    if (relax_edge(u, e.to, e.weight)) {
                        if (db[u] + e.weight < B) {
                            W_i.insert(e.to);
                            W.insert(e.to);
                        }
                    }
                }
            }
            W_i_minus_1 = W_i;

            if (static_cast<int>(W.size()) > k * static_cast<int>(S.size())) {
                return {S, W};
            }
        }

        std::vector<std::pair<int, int>> F_edges;
        for (int u : W) {
            for (const Edge& e : graph.adj[u]) {
                if (W.contains(e.to) && abs(db[e.to] - (db[u] + e.weight)) < EPS) {
                    F_edges.emplace_back(u, e.to);
                }
            }
        }

        std::unordered_map<int, int> tree_size;
        std::unordered_set<int> visited;

        std::function<int(int)> dfs_count = [&](const int u) -> int {
            if (visited.contains(u)) return 0;

            visited.insert(u);
            int count = 1;
            for (auto& [fu, fv] : F_edges) {
                if (fu == u) {
                    count += dfs_count(fv);
                }
            }
            return count;
        };

        std::set<int> P;
        for (int u : S) {
            if (!visited.contains(u)) {
                int size = dfs_count(u);
                tree_size[u] = size;
                if (size >= k) {
                    P.insert(u);
                }
            }
        }

        return {P, W};
    }

    std::pair<double, std::set<int>> base_case(double B, const std::set<int>& S) {
        int x = *S.begin();
        std::set<int> U0 = {x};

        std::priority_queue<std::pair<double, int>, std::vector<std::pair<double, int>>, std::greater<>> heap;
        heap.emplace(db[x], x);
        std::set<int> visited;

        while (!heap.empty() && static_cast<int>(U0.size()) < k + 1) {
            auto [dist, u] = heap.top();
            heap.pop();

            if (visited.contains(u)) continue;
            visited.insert(u);
            if (u != x) U0.insert(u);

            for (const Edge& e : graph.adj[u]) {
                if (relax_edge(u, e.to, e.weight) && db[u] + e.weight < B) {
                    if (!visited.contains(e.to)) {
                        heap.emplace(db[e.to], e.to);
                    }
                }
            }
        }

        if (static_cast<int>(U0.size()) <= k) {
            return {B, U0};
        }
        double B_prime = -INF;
        for (const int v : U0) {
            B_prime = std::max(B_prime, db[v]);
        }
        std::set<int> U;
        for (int v : U0) {
            if (db[v] < B_prime) {
                U.insert(v);
            }
        }
        return {B_prime, U};
    }
};

#endif //SMALLCPPPROGRAM_BMSSP_H