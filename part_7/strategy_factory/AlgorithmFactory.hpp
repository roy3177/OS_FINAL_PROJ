#pragma once
#include <memory>
#include <string>
#include "IAlgorithm.hpp"
#include "MaxFlowAlgo.hpp"
#include "CliquesAlgo.hpp"
#include "SCCAlgo.hpp"
#include "MSTAlgo.hpp"
#include <algorithm>

class AlgorithmFactory 
{
public:
    static std::unique_ptr<IAlgorithm> create(const std::string& id);
};
