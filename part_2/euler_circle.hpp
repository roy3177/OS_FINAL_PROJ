/*
@author: Roy Meoded
@author: Yarin Keshet  
@ date: 10-10-2025

@ description: This file contains the implementation of the EulerCircle class, 
which provides functionality to find an Eulerian circuit in a given graph using Hierholzer's algorithm.
*/

#pragma once

#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include "../part_1/graph_impl.hpp"

class EulerCircle 
{
public:
    EulerCircle(const Graph& graph) : g(graph) {}

    void findEulerianCircuit(); 

    const Graph& g;

    // Hierholzer's algorithm for Eulerian circuit
    void hierholzer(std::vector<std::vector<int>>& adjList, int start, std::vector<int>& circuit);
};