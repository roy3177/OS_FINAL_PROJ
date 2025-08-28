#include "../part_1/graph_impl.hpp"
#include "../part_2/euler_circle.hpp"
#include "graph_algos.hpp"
#include "Finding_Max_Flow.hpp"
#include "Finding_Num_Cliques.hpp"
#include "Finding_SCC.hpp"
#include "MST_Weight.hpp"

int main()
{
    // --- Finding Max Flow ---
    std::cout << "--- Finding Max Flow ---\n";
    // Example: Create a directed graph with 4 vertices
    Graph g(4, true);

    // Add edges with capacities
    g.addEdge(0, 1, 3); // from 0 to 1 with capacity 3
    g.addEdge(0, 2, 2); // from 0 to 2 with capacity 2
    g.addEdge(1, 2, 5); // from 1 to 2 with capacity 5
    g.addEdge(1, 3, 2); // from 1 to 3 with capacity 2
    g.addEdge(2, 3, 3); // from 2 to 3 with capacity 3

    // Run max flow from source (0) to sink (3)
    FindingMaxFlow algo;
    int maxFlow = algo.findMaxFlow(g, 0, 3);

    std::cout << "Max flow from 0 to 3: " << maxFlow << std::endl;

    // --- Finding Number of Cliques ---
    std::cout << "\n--- Finding Number of Cliques ---\n";
    FindingNumCliques cliqueFinder;
    int k = 3; // Size of cliques to find
    int numCliques = cliqueFinder.countCliques(g, k);
    std::cout << "k = " << k << std::endl;
    std::cout << "Number of " << k << "-cliques: " << numCliques << std::endl;

    // --- Finding SCC ---
    std::cout << "\n--- Finding Strongly Connected Components (SCC) ---\n";
    FindingSCC sccFinder;
    auto sccs = sccFinder.findSCCs(g);
    std::cout << "Number of strongly connected components: " << sccs.size() << std::endl;
    for (const auto& component : sccs) 
    {
        std::cout << "SCC: ";
        for (int v : component) {
            std::cout << v << " ";
        }
        std::cout << std::endl;
    }

    // --- MST Weight ---
    std::cout << "\n--- Finding Minimum Spanning Tree (MST) Weight ---\n";
    MSTWeight mstFinder;
    int mstWeight = mstFinder.findMSTWeight(g);
    std::cout << "MST weight: " << mstWeight << std::endl;

    return 0;
}