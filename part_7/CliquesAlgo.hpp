#pragma once
#include "Algorithm.hpp"
#include "Finding_Num_Cliques.hpp"

class CliquesAlgo : public IAlgorithm {
public:
    std::string id() const override { return "CLIQUES"; }
    std::string run(const Graph& g, const std::unordered_map<std::string,int>& params) override {
        int k = params.count("K") ? params.at("K") : 3;
        FindingNumCliques algo;
        int res = algo.countCliques(g, k);
        return "RESULT " + std::to_string(res);
    }
};
