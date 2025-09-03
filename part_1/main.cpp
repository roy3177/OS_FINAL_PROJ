#include <iostream>
#include "graph_impl.hpp"

int main() 
{
    Graph g(5, false);

    // Valid edges
    g.addEdge(0, 1);
    g.addEdge(0, 4);

    // Invalid edge (should throw)
    try {
        g.addEdge(-1, 2);
    } catch (const std::out_of_range& e) {
        std::cout << "Caught exception: " << e.what() << std::endl;
    }

    // Valid get_neighbors
    const auto& neighbors = g.get_neighbors(0);
    std::cout << "Neighbors of 0: ";
    for (int v : neighbors) std::cout << v << " ";
    std::cout << std::endl;

    // Invalid get_neighbors (should throw)
    try {
        g.get_neighbors(10);
    } catch (const std::out_of_range& e) {
        std::cout << "Caught exception: " << e.what() << std::endl;
    }

    return 0;
}