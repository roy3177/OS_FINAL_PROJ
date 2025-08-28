#include "AlgorithmFactory.hpp"
#include "MaxFlowAlgo.hpp"
#include "CliquesAlgo.hpp"
#include "SCCAlgo.hpp"
#include "MSTAlgo.hpp"

#include <algorithm>

std::unique_ptr<IAlgorithm> AlgorithmFactory::create(const std::string& id) {
    std::string up = id;
    std::transform(up.begin(), up.end(), up.begin(), ::toupper);
    if (up == "MAX_FLOW") return std::make_unique<MaxFlowAlgo>();
    if (up == "CLIQUES") return std::make_unique<CliquesAlgo>();
    if (up == "SCC") return std::make_unique<SCCAlgo>();
    if (up == "MST") return std::make_unique<MSTAlgo>();
    return nullptr;
}
