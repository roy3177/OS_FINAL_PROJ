#pragma once

#include "../part_1/graph_impl.hpp"

#include <vector>
#include <algorithm>

class FindingNumCliques 
{
public:
    // Counts the number of cliques of size k in the given graph
    int countCliques(const Graph& graph, int k);
};