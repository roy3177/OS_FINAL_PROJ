#include <iostream>
#include "../part_1/graph_impl.hpp"
#include "../part_2/euler_circle.hpp"

// Example usage
int main() 
{
    // ===== Case A: Original graph (Eulerian) =====
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

        std::cout << "Graph adjacency list (Case A):\n";
        g.print();

        std::cout << "\nChecking for Eulerian circuit (Case A):\n";
        EulerCircle ec(g);
        ec.findEulerianCircuit();
    }

    // ===== Case B: Disconnected graph (not Eulerian) =====
    {
        Graph g(6, false);
        g.addEdge(0, 1); g.addEdge(1, 2); g.addEdge(2, 0); // triangle 1
        g.addEdge(3, 4); g.addEdge(4, 5); g.addEdge(5, 3); // triangle 2

        std::cout << "\nGraph adjacency list (Case B):\n";
        g.print();

        std::cout << "\nChecking for Eulerian circuit (Case B):\n";
        EulerCircle ec(g);
        ec.findEulerianCircuit();
    }

    // ===== Case C: Graph with odd degree vertices =====
    {
        Graph g(4, false);
        g.addEdge(0, 1);
        g.addEdge(1, 2);
        g.addEdge(2, 3);

        std::cout << "\nGraph adjacency list (Case C):\n";
        g.print();

        std::cout << "\nChecking for Eulerian circuit (Case C):\n";
        EulerCircle ec(g);
        ec.findEulerianCircuit();
    }

    // ===== Case D: Empty graph (no edges) =====
    {
        Graph g(5, false);

        std::cout << "\nGraph adjacency list (Case D):\n";
        g.print();

        std::cout << "\nChecking for Eulerian circuit (Case D):\n";
        EulerCircle ec(g);
        ec.findEulerianCircuit();
    }

    return 0;
}
