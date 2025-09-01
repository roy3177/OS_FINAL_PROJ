#include "../include/random_graph.hpp"
#include <random>
#include <set>

Graph generate_random_graph(int vertices, int edges, int seed, bool directed, int wmin, int wmax)
{
    if (wmin > wmax) std::swap(wmin, wmax);
    if (wmin < 1) wmin = 1; // keep positive weights/capacities
    Graph g(vertices, directed);
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> distV(0, vertices - 1);
    std::uniform_int_distribution<int> distW(wmin, wmax);

    if (!directed) {
        std::set<std::pair<int, int>> used_edges;
        int added = 0;
        while (added < edges)
        {
            int u = distV(rng);
            int v = distV(rng);
            if (u == v) continue;
            auto e = std::make_pair(std::min(u,v), std::max(u,v));
            if (used_edges.count(e)) continue;
            int w = distW(rng);
            g.addEdge(u, v, w);
            used_edges.insert(e);
            ++added;
        }
    } else {
        std::set<long long> used;
        auto key = [vertices](int u, int v){ return (long long)u * (vertices + 1LL) + v; };
        int added = 0;
        while (added < edges)
        {
            int u = distV(rng);
            int v = distV(rng);
            if (u == v) continue;
            long long k = key(u,v);
            if (used.count(k)) continue;
            int w = distW(rng);
            g.addEdge(u, v, w);
            used.insert(k);
            ++added;
        }
    }
    return g;
}
