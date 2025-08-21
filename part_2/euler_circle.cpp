#include "euler_circle.hpp"

void EulerCircle::findEulerianCircuit() 
{
    std::vector<std::vector<int>> adjList = g.getAdjList();
    int V = adjList.size();
    bool isEulerian = true;
    // Check for undirected graph: all degrees must be even
    for (int u = 0; u < V; ++u) 
    {
        if (adjList[u].size() % 2 != 0) 
        {
            isEulerian = false;
            std::cout << "Vertex " << u << " has odd degree: " << adjList[u].size() << std::endl;
        }
    }
    if (!isEulerian) 
    {
        std::cout << "No Eulerian circuit exists: not all vertices have even degree." << std::endl;
        return;
    }

    // If all degrees are even, run Hierholzer's algorithm
    std::vector<int> circuit;
    hierholzer(adjList, 0, circuit);
    // Print the Eulerian circuit
    std::cout << "Eulerian circuit: ";
    for (int v : circuit) 
    {
        std::cout << v << " ";
    }
    std::cout << std::endl;
}

void EulerCircle::hierholzer(std::vector<std::vector<int>>& adjList, int start, std::vector<int>& circuit) 
{
    std::vector<int> stack;
    stack.push_back(start);
    while (!stack.empty()) 
    {
        int u = stack.back();
        if (!adjList[u].empty()) 
        {
            int v = adjList[u].back();
            adjList[u].pop_back();
            // Remove the edge in the other direction for undirected graphs
            for (auto it = adjList[v].begin(); it != adjList[v].end(); ++it) 
            {
                if (*it == u) 
                {
                    adjList[v].erase(it);
                    break;
                }
            }
            stack.push_back(v);
        }
        else 
        {
            circuit.push_back(u);
            stack.pop_back();
        }
    }
    std::reverse(circuit.begin(), circuit.end());
}
