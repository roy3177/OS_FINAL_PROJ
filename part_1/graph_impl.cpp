#include "graph_impl.hpp"

void Graph::addEdge(int u, int v, int cap) 
{
<<<<<<< HEAD
    if (u < 0 || u >= V || v < 0 || v >= V) {
        throw std::out_of_range("Vertex index out of range");
    }
    if (cap < 0) {
        throw std::invalid_argument("capacity must be non-negative");
    }

=======
>>>>>>> 72e67a084588fb1a6763f419837d84db6094d906
    adj[u].push_back(v);
    capacity[u][v] = cap;
    if (!directed) 
    {
        adj[v].push_back(u);
        capacity[v][u] = cap;
    }
    ++E;
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

bool Graph::is_edge(int u, int v) const 
{
    const auto& neighbors = get_neighbors(u);
    return std::find(neighbors.begin(), neighbors.end(), v) != neighbors.end();
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