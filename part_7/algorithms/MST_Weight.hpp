#pragma once

#include "../part_1/graph_impl.hpp"

#include <vector>
#include <algorithm>

class MSTWeight 
{
public:
    // Returns the total weight of the MST
    int findMSTWeight(const Graph& graph);
};
