#pragma once
#include "IAlgorithm.hpp"
#include "Finding_SCC.hpp"

class SCCAlgo : public IAlgorithm {
public:
    std::string id() const override { return "SCC"; }
    std::string run(const Graph& g, const std::unordered_map<std::string,int>&) override {
        FindingSCC algo;
        auto sccs = algo.findSCCs(g);
        return "RESULT " + std::to_string((int)sccs.size());
    }
};
