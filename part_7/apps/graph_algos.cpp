
#include "graph_algos.hpp"

int main()
{
    std::cout << "***************************************************************************************************" << std::endl;
    
    // Example: Create a directed graph with 4 vertices
    Graph g_directed(4, true);

    // Add edges with capacities
    g_directed.addEdge(0, 1, 3); // from 0 to 1 with capacity 3
    g_directed.addEdge(0, 2, 2); // from 0 to 2 with capacity 2
    g_directed.addEdge(1, 2, 5); // from 1 to 2 with capacity 5
    g_directed.addEdge(1, 3, 2); // from 1 to 3 with capacity 2
    g_directed.addEdge(2, 3, 3); // from 2 to 3 with capacity 3

    std::cout << "Directed graph adjacency list:\n";
    g_directed.print();
    std::cout << "***************************************************************************************************" << std::endl;
    std::cout <<"---------------------------------------------------------------------------------------------------"<< std::endl;

    // --- Finding SCC ---
    std::cout << "\n--- Finding Strongly Connected Components (SCC) ---\n";
    FindingSCC sccFinder;
    auto sccs = sccFinder.findSCCs(g_directed);
    std::cout << "Number of strongly connected components: " << sccs.size() << std::endl;
    for (const auto& component : sccs) 
    {
        std::cout << "SCC: ";
        for (int v : component) 
        {
            std::cout << v << " ";
        }
        std::cout << std::endl;
    }
    std::cout <<"---------------------------------------------------------------------------------------------------"<< std::endl;

    // --- Finding Max Flow ---
    std::cout << "--- Finding Max Flow ---\n";

    // Run max flow from source (0) to sink (3)
    FindingMaxFlow algo;
    int maxFlow = algo.findMaxFlow(g_directed, 0, 3);

    std::cout << "Max flow from 0 to 3: " << maxFlow << std::endl;
    std::cout <<"---------------------------------------------------------------------------------------------------"<< std::endl;

    std::cout << "***************************************************************************************************" << std::endl;

    // Example: Create an undirected graph with 4 vertices:
    Graph g_undirected(4, false);

    //note: weight is only relevant for the MST algo.
    g_undirected.addEdge(0, 1, 2);
    g_undirected.addEdge(1, 2, 3);
    g_undirected.addEdge(2, 3, 3);
    g_undirected.addEdge(3, 0, 5);
    g_undirected.addEdge(0, 2, 6);

    std::cout << "Undirected graph adjacency list:\n";
    g_undirected.print();
    std::cout << "***************************************************************************************************" << std::endl;
    std::cout <<"---------------------------------------------------------------------------------------------------"<< std::endl;

    // --- Finding Number of Cliques ---
    std::cout << "\n--- Finding Number of Cliques ---\n";
    FindingNumCliques cliqueFinder;
    int k = 3; // Size of cliques to find
    int numCliques = cliqueFinder.countCliques(g_undirected, k);
    std::cout << "k = " << k << std::endl;
    std::cout << "Number of " << k << "-cliques: " << numCliques << std::endl;
    std::cout <<"---------------------------------------------------------------------------------------------------"<< std::endl;

    // --- MST Weight ---
    std::cout << "\n--- Finding Minimum Spanning Tree (MST) Weight ---\n";
    MSTWeight mstFinder;
    int mstWeight = mstFinder.findMSTWeight(g_undirected);
    std::cout << "MST weight: " << mstWeight << std::endl;

    return 0;
}