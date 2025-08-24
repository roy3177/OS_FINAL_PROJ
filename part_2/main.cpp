#include <iostream>
#include "../part_1/graph_impl.hpp"
#include "../part_2/euler_circle.hpp"

// Example usage
int main() 
{
    Graph g(8, false);

    g.addEdge(0, 1);
    g.addEdge(0, 7);
    g.addEdge(0, 6);
    g.addEdge(0, 2);
    g.addEdge(6, 4);
    g.addEdge(6, 7);
    g.addEdge(6, 5);
    g.addEdge(4, 1);
    g.addEdge(4, 5);
    g.addEdge(4, 3);
    g.addEdge(2, 1);
    g.addEdge(2, 5);
    g.addEdge(3, 2);
    g.addEdge(1, 5);

    std::cout << "Graph adjacency list:\n";
    g.print();

    std::cout << "\nChecking for Eulerian circuit:\n";
    EulerCircle ec(g);
    ec.findEulerianCircuit();
    return 0;
}