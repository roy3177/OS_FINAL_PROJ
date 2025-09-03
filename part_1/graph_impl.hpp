#pragma once

#include <iostream>
#include <vector>
#include <stdexcept>
#include <algorithm>


class Graph 
{
private:
    int V;  // number of vertices
    int E;  // number of edges
    bool directed; // default is undirected
    std::vector<std::vector<int>> adj; // adjacency list
    std::vector<std::vector<int>> capacity; // capacity[u][v] = capacity of edge u->v

public:
// Constructor:
/*
The vertices number passed to adj creates a vector with empty vector lists,
the size of the vertices we have(each vertex represent an empty list of its neighbors)
*/
Graph(int vertices, bool isDirected) : 
<<<<<<< HEAD
    V(vertices), E(0), directed(isDirected), adj(vertices), capacity(vertices, std::vector<int>(vertices, 0)) {
        if (vertices <= 0) {
            throw std::invalid_argument("number of vertices must be positive");
        }
    }
=======
    V(vertices), E(0), directed(isDirected), adj(vertices), capacity(vertices, std::vector<int>(vertices, 0)) {}
>>>>>>> 72e67a084588fb1a6763f419837d84db6094d906

    // Add edge (u -> v)
    void addEdge(int u, int v, int cap = 1); // default capacity is 1 if not specified


    // Get number of vertices
    int get_vertices() const { return V; }

    // Get number of edges

    // Return the full adjacency list
    const std::vector<std::vector<int>>& getAdjList() const { return adj; }
    
    // Get number of edges
    int get_edges() const { return E; }

    // Return adjacency list of a vertex
    const std::vector<int>& get_neighbors(int u) const;

    // Getter for capacity
    const std::vector<std::vector<int>>& get_capacity() const { return capacity; }

    // Returns true if there is an edge from u to v
    bool is_edge(int u, int v) const;

    // Print graph (for debugging)
    void print() const;


};


