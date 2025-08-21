#pragma once

#include <iostream>
#include <vector>
#include <stdexcept>
#include "graph_impl.hpp"


class Graph 
{
private:
    int V;  // number of vertices
    int E;  // number of edges
    bool directed; // default is undirected
    std::vector<std::vector<int>> adj; // adjacency list

public:
// Constructor:
/*
The vertices number passed to adj creates a vector with empty vector lists,
the size of the vertices we have(each vertex represent an empty list of its neighbors)
*/
    Graph(int vertices, bool isDirected) : 
    V(vertices), E(0), directed(isDirected), adj(vertices) {}

    // Add edge (u -> v)
    void addEdge(int u, int v);


    // Get number of vertices
    int get_vertices() const { return V; }

    // Get number of edges

    // Return the full adjacency list
    const std::vector<std::vector<int>>& getAdjList() const { return adj; }
    
    // Get number of edges
    int get_edges() const { return E; }

    // Return adjacency list of a vertex
    const std::vector<int>& get_neighbors(int u) const;

    // Print graph (for debugging)
    void print() const;
};


