#pragma once
#include <memory>
#include <string>
#include "IAlgorithm.hpp"

class AlgorithmFactory 
{
public:
    static std::unique_ptr<IAlgorithm> create(const std::string& id);
};
