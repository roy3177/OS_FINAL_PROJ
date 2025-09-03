#pragma once
#include "IAlgorithm.hpp"
#include "MST_Weight.hpp"

class MSTAlgo : public IAlgorithm 
{
public:
    std::string id() const override 
    { 
        return "MST"; 
    }
    std::string run(const Graph& g, const std::unordered_map<std::string,int>&) override 
    {
        MSTWeight algo; // Instantiates the algorithm class
        int res = algo.findMSTWeight(g); // Executes the algorithm
        return "RESULT " + std::to_string(res); // Returns the result
    }
};
