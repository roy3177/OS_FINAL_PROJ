#pragma once

#include "../part_1/graph_impl.hpp"
#include <vector>
#include <stack>
#include <algorithm>

class FindingSCC 
{
public:
    // Returns a vector of SCCs, each SCC is a vector of vertex indices
    std::vector<std::vector<int>> findSCCs(const Graph& graph);
};