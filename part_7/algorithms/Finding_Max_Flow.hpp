#pragma once

#include "../part_1/graph_impl.hpp"
#include <queue>
#include <vector>
#include <climits>

class FindingMaxFlow 
{
public:
    int findMaxFlow(Graph& g, int source, int sink);
};