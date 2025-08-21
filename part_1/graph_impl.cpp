#include "graph_impl.hpp"




void Graph::addEdge(int u, int v) 
{
    if (u < 0 || v < 0 || u >= V || v >= V) 
    {
        throw std::out_of_range("Vertex index out of range");
    }

    adj[u].push_back(v);
    if (!directed) 
    {
        adj[v].push_back(u);
    }
    E++;
}

// Return adjacency list of a vertex
const std::vector<int>& Graph::get_neighbors(int u) const 
{
    if (u < 0 || u >= V) 
    {
        throw std::out_of_range("Vertex index out of range");
    }
    return adj[u];
}

// Print graph (for debugging)
void Graph::print() const 
{
    for (int u = 0; u < V; u++) 
    {
        std::cout << u << ": ";
        for (int v : adj[u]) 
        {
            std::cout << v << " ";
        }
        std::cout << "\n";
    }
}