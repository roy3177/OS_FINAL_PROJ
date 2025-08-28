#pragma once

#include <string>
#include <unordered_map>

#include "../part_1/graph_impl.hpp"

// Strategy interface for algorithms
class IAlgorithm {
public:
    virtual ~IAlgorithm() = default;
    virtual std::string id() const = 0;
    virtual std::string run(const Graph& g, const std::unordered_map<std::string, int>& params) = 0;
};
