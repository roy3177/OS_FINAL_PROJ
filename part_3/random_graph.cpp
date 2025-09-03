#include "random_graph.hpp"
#include <random>
#include <set>

Graph generate_random_graph(int vertices, int edges, int seed) 
{
    Graph g(vertices, false); // undirected by default
    /*
    Generates a random graph with the specified number of vertices and edges.
    */
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> dist(0, vertices - 1);

    std::set<std::pair<int, int>> used_edges;
    int added = 0;
    while (added < edges) 
    {
        /*
        Generate random edge (u, v)
        Ensure no self-loops (u != v) and no duplicate edges
        make_pair creates a pair representing an undirected edge between vertices u and v,
        always storing the smaller vertex first.
        */
        int u = dist(rng);
        int v = dist(rng);
        if (u == v) continue; // no self-loops
        auto edge = std::make_pair(std::min(u, v), std::max(u, v));
        if (used_edges.count(edge)) continue; // avoid duplicate edges
        g.addEdge(u, v);
        used_edges.insert(edge);
        ++added;
    }
    return g;
}