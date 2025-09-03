#include "../include/random_graph.hpp"

/*
    Generate a simple random graph with positive edge weights in [wmin, wmax].
    - vertices: number of vertices (0..vertices-1)
    - edges: number of distinct edges to add
    - seed: PRNG seed to make generation deterministic/repeatable
    - directed: if true, produce directed edges (u->v); otherwise undirected
    - wmin/wmax: inclusive weight range; wmin is normalized to be at least 1

    Guarantees:
    - No self-loops (u != v)
    - No duplicate edges: for undirected we avoid adding both (u,v) and (v,u);
    for directed we avoid duplicate (u->v).
*/
Graph generate_random_graph(int vertices, int edges, int seed, bool directed, int wmin, int wmax)
{
    // Normalize weight range (ensure wmin <= wmax and wmin >= 1)
    if (wmin > wmax) 
    {
        std::swap(wmin, wmax);
    }
    if (wmin < 1)
    {
        wmin = 1; // keep positive weights/capacities
    } 

    // Prepare an empty graph with the requested orientation
    Graph g(vertices, directed);

    // RNG setup: deterministic PRNG seeded by 'seed'
    std::mt19937 rng(seed);
    // Uniformly pick endpoints in [0, vertices-1]
    std::uniform_int_distribution<int> distV(0, vertices - 1);
    // Uniformly pick weights in [wmin, wmax]
    std::uniform_int_distribution<int> distW(wmin, wmax);

    if (!directed) 
    {
        // Undirected: track used unordered pairs (min(u,v), max(u,v)) to avoid duplicates
        std::set<std::pair<int, int>> used_edges;
        int added = 0;
        while (added < edges)
        {
            int u = distV(rng);
            int v = distV(rng);
            if (u == v)
            {
                continue; // skip self-loops
            }
            auto e = std::make_pair(std::min(u,v), std::max(u,v));
            if (used_edges.count(e))
            {
                continue; // already have this undirected edge
            }
            int w = distW(rng);
            g.addEdge(u, v, w);
            used_edges.insert(e);
            ++added;
        }
    }
    else 
    {
        // Directed: track used ordered pairs via a compact 64-bit key (u,v)
        std::set<long long> used;
        // lambda for generating unique keys for edges:
        auto key = [vertices](int u, int v)
        {
             return (long long)u * (vertices + 1LL) + v; 
        };
        int added = 0;
        while (added < edges)
        {
            int u = distV(rng);
            int v = distV(rng);
            if (u == v)
            {
                continue; // skip self-loops
            } 
            long long k = key(u,v);
            if (used.count(k))
            {
                continue; // already have u->v
            }
            int w = distW(rng);
            g.addEdge(u, v, w);
            used.insert(k);
            ++added;
        }
    }
    // Return the generated graph
    return g;
}
