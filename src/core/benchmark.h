#ifndef CORE_BENCHMARK_H
#define CORE_BENCHMARK_H

#include <string>

namespace core {

class Benchmark {
public:
    Benchmark() = default;

    Benchmark(std::string id,
              std::string name,
              float approxTime,
              float approxMemory,
              std::string sampleId,
              std::string alg)
        : id_(std::move(id)),
          name_(std::move(name)),
          approxTime_(approxTime),
          approxMemory_(approxMemory),
          sampleId_(std::move(sampleId)),
          alg_(std::move(alg)) {}

    const std::string& id() const { return id_; }

    const std::string& name() const { return name_; }

    float getTime() const { return approxTime_; }

    float getMemory() const { return approxMemory_; }

    const std::string& sampleId() const { return sampleId_; }

    const std::string& alg() const { return alg_; }

private:
    std::string id_;
    std::string name_;
    float approxTime_{0.0f};
    float approxMemory_{0.0f};
    std::string sampleId_;
    std::string alg_;
};

} // namespace core

#endif // CORE_BENCHMARK_H

