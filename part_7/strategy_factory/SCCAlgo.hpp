#pragma once
#include "IAlgorithm.hpp"
#include "Finding_SCC.hpp"

class SCCAlgo : public IAlgorithm 
{
public:
    std::string id() const override 
    {
        return "SCC"; 
    }
    std::string run(const Graph& g, const std::unordered_map<std::string,int>&) override 
    {
        FindingSCC algo; // Instantiates the algorithm class
        auto sccs = algo.findSCCs(g); // Executes the algorithm
        return "RESULT " + std::to_string((int)sccs.size()); // Returns the result
    }
};
