#pragma once
#include "IAlgorithm.hpp"
#include "Finding_Max_Flow.hpp"

class MaxFlowAlgo : public IAlgorithm 
{
public:
    std::string id() const override { return "MAX_FLOW"; }
    std::string run(const Graph& g, const std::unordered_map<std::string,int>& params) override {
        int src = params.count("SRC") ? params.at("SRC") : 0;
        int sink = params.count("SINK") ? params.at("SINK") : g.get_vertices()-1;
        FindingMaxFlow algo;
        Graph gCopy = g; // make a modifiable copy for algorithms that mutate the graph
        int res = algo.findMaxFlow(gCopy, src, sink);
        return "RESULT " + std::to_string(res);
    }
};
