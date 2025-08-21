#include <iostream>
#include "graph_impl.hpp"



// Example usage
int main() 
{
    Graph g(5, false); // 5 vertices, undirected

    g.addEdge(0, 1);
    g.addEdge(0, 4);
    g.addEdge(1, 2);
    g.addEdge(1, 3);
    g.addEdge(1, 4);
    g.addEdge(2, 3);
    g.addEdge(3, 4);

    std::cout << "Graph adjacency list:\n";
    g.print();

    return 0;
}