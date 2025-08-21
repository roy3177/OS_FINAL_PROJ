#include <iostream>
#include "../part_1/graph_impl.hpp"
#include "../part_2/euler_circle.hpp"

// Example usage
int main() 
{
    Graph g(4, false);

    g.addEdge(0, 1);
    g.addEdge(1, 2);
    g.addEdge(2, 3);
    g.addEdge(3, 0);

    std::cout << "Graph adjacency list:\n";
    g.print();

    std::cout << "\nChecking for Eulerian circuit:\n";
    EulerCircle ec(g);
    ec.findEulerianCircuit();
    return 0;
}