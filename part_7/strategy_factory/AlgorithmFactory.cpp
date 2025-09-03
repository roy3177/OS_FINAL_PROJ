#include "AlgorithmFactory.hpp"

/*
Purpose: a simple factory that returns the right algorithm object based on a string ID.

Steps:
* Copies id to up and uppercases it (case-insensitive matching).
* Compares up to known names: MAX_FLOW, CLIQUES, SCC, MST.
* For a match, returns a std::unique_ptr to the corresponding adapter (e.g., MaxFlowAlgo).
* If no match, returns nullptr
*/
std::unique_ptr<IAlgorithm> AlgorithmFactory::create(const std::string& id) 
{
    std::string up = id;
    std::transform(up.begin(), up.end(), up.begin(), ::toupper); // Uppercases the whole id string in-place so matching is case-insensitive
    if (up == "MAX_FLOW") return std::make_unique<MaxFlowAlgo>();
    if (up == "CLIQUES") return std::make_unique<CliquesAlgo>();
    if (up == "SCC") return std::make_unique<SCCAlgo>();
    if (up == "MST") return std::make_unique<MSTAlgo>();
    return nullptr;
}
