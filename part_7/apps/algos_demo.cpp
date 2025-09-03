
#include "algos_demo.hpp"

int main()
{
    std::cout << "***************************************************************************************************" << std::endl;
    
    Graph g_directed_1(6, true);

    g_directed_1.addEdge(0, 1, 11);
    g_directed_1.addEdge(0, 2, 12);
    g_directed_1.addEdge(2, 1, 1);
    g_directed_1.addEdge(1, 3, 12);
    g_directed_1.addEdge(2, 4, 11);
    g_directed_1.addEdge(4, 3, 7);
    g_directed_1.addEdge(4, 5, 4);
    g_directed_1.addEdge(3, 5, 19);

    std::cout << "Directed graph adjacency list for max flow:\n";
    g_directed_1.print();
    std::cout << "***************************************************************************************************" << std::endl;

    std::cout <<"---------------------------------------------------------------------------------------------------"<< std::endl;

    // --- Finding Max Flow ---
    std::cout << "--- Finding Max Flow ---\n";

    // Run max flow from source (0) to sink (5)
    FindingMaxFlow algo;
    int maxFlow = algo.findMaxFlow(g_directed_1, 0, 5);

    std::cout << "Max flow from 0 to 5: " << maxFlow << std::endl;
    std::cout <<"---------------------------------------------------------------------------------------------------"<< std::endl;
    std::cout << "***************************************************************************************************" << std::endl;
    
    Graph g_directed_2(7, true);

    g_directed_2.addEdge(0, 1);
    g_directed_2.addEdge(1, 2);
    g_directed_2.addEdge(2, 3);
    g_directed_2.addEdge(3, 0);
    g_directed_2.addEdge(1, 3);

    g_directed_2.addEdge(3, 4);
    g_directed_2.addEdge(2, 5);

    g_directed_2.addEdge(4, 5);
    g_directed_2.addEdge(5, 6);
    g_directed_2.addEdge(6, 4);


    std::cout << "Directed graph adjacency list for SCC:\n";
    g_directed_2.print();
    std::cout << "***************************************************************************************************" << std::endl;

    std::cout <<"---------------------------------------------------------------------------------------------------"<< std::endl;

    // --- Finding SCC ---
    std::cout << "\n--- Finding Strongly Connected Components (SCC) ---\n";
    FindingSCC sccFinder;
    auto sccs = sccFinder.findSCCs(g_directed_2);
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

    std::cout << "***************************************************************************************************" << std::endl;

    Graph g_undirected_1(6, false);

    g_undirected_1.addEdge(0, 4);
    g_undirected_1.addEdge(4, 3);
    g_undirected_1.addEdge(3, 2);
    g_undirected_1.addEdge(2, 0);

    g_undirected_1.addEdge(2, 1);
    g_undirected_1.addEdge(1, 3);
    g_undirected_1.addEdge(1, 0);
    g_undirected_1.addEdge(1, 5);

    g_undirected_1.addEdge(3, 5);
    g_undirected_1.addEdge(2, 5);

    std::cout << "Undirected graph adjacency list for finding number of cliques:\n";
    g_undirected_1.print();
    std::cout << "***************************************************************************************************" << std::endl;
    std::cout <<"---------------------------------------------------------------------------------------------------"<< std::endl;

    // --- Finding Number of Cliques ---
    std::cout << "\n--- Finding Number of Cliques ---\n";
    FindingNumCliques cliqueFinder;
    int k = 3; // Size of cliques to find
    int numCliques = cliqueFinder.countCliques(g_undirected_1, k);
    std::cout << "k = " << k << std::endl;
    std::cout << "Number of " << k << "-cliques: " << numCliques << std::endl;
    std::cout <<"---------------------------------------------------------------------------------------------------"<< std::endl;

    std::cout << "***************************************************************************************************" << std::endl;

    Graph g_undirected_2(9, false);

    g_undirected_2.addEdge(0, 1, 4);
    g_undirected_2.addEdge(0, 7, 8);
    g_undirected_2.addEdge(1, 7, 11);

    g_undirected_2.addEdge(1, 2, 8);
    g_undirected_2.addEdge(7, 6, 1);
    g_undirected_2.addEdge(7, 8, 7);

    g_undirected_2.addEdge(2, 8, 2);
    g_undirected_2.addEdge(8, 6, 6);
    g_undirected_2.addEdge(6, 5, 2);
    g_undirected_2.addEdge(2, 3, 7);
    g_undirected_2.addEdge(2, 5, 4);

    g_undirected_2.addEdge(3, 5, 14);
    g_undirected_2.addEdge(3, 4, 9);
    g_undirected_2.addEdge(5, 4, 10);

    std::cout << "Undirected graph adjacency list for MST:\n";
    g_undirected_2.print();
    std::cout << "***************************************************************************************************" << std::endl;


    // --- MST Weight ---
    std::cout << "\n--- Finding Minimum Spanning Tree (MST) Weight ---\n";
    MSTWeight mstFinder;
    int mstWeight = mstFinder.findMSTWeight(g_undirected_2);
    std::cout << "MST weight: " << mstWeight << std::endl;

    return 0;
}