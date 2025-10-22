#include "Finding_SCC.hpp"


/*
This function performs a depth-first search (DFS) starting from vertex v:
*It marks v as visited.
*For each neighbor u of v, if u is not visited, it recursively calls itself on u.
*After visiting all reachable vertices from v, it pushes v onto the stack order.

Purpose:
It records the finishing order of vertices for Kosaraju's algorithm.
Vertices are pushed onto the stack after all their descendants are visited,
so the stack ends up with vertices ordered by their finishing times (used to find SCCs in the transposed graph).
*/
static void dfsFillOrder(const Graph& graph, int v, std::vector<bool>& visited, std::stack<int>& order) 
{
	visited[v] = true; // Mark the current node as visited

	// Recur for all the vertices adjacent to this vertex(v):
	for (int u : graph.get_neighbors(v)) 
    {
		if (!visited[u]) 
        {
			dfsFillOrder(graph, u, visited, order); // Recursive call
		}
	}
	order.push(v); // Push v to stack after visiting all its neighbors
}

/*
This function performs a depth-first search (DFS) on the transposed graph (G^t) , starting from vertex v:
*It marks v as visited.
*Adds v to the current strongly connected component (component).
*For each neighbor u of v in the transposed graph, if u is not visited, it recursively calls itself on u.

Purpose:
It collects all vertices reachable from v in the transposed graph,
forming one strongly connected component (SCC).
Each call finds and records a complete SCC.
*/
static void dfsOnTranspose(const std::vector<std::vector<int>>& transpose, int v, std::vector<bool>& visited, std::vector<int>& component) \
{
	visited[v] = true;
	component.push_back(v);
	for (int u : transpose[v]) 
    {
		if (!visited[u]) 
        {
			dfsOnTranspose(transpose, u, visited, component); // Recursive call
		}
	}
}

/*
This function, FindingSCC::findSCCs, implements Kosaraju's algorithm to find all strongly connected components (SCCs) in a directed graph:
1. Perform a DFS on the original graph to determine the finishing order of vertices (dfsFillOrder).
2. Create a transposed graph by reversing the direction of all edges.
3. Perform a DFS on the transposed graph in the order defined by the finishing times to identify SCCs (dfsOnTranspose).

*Each SCC is stored as a vector of vertex indices in the result. The function returns a list of all SCCs found in the graph.
*/
std::vector<std::vector<int>> FindingSCC::findSCCs(const Graph& graph) 
{
	int n = graph.get_vertices();
	std::vector<bool> visited(n, false);
	std::stack<int> order;

	// 1. Fill vertices in stack according to their finishing times
	for (int v = 0; v < n; ++v) 
    {
		if (!visited[v]) 
        {
			dfsFillOrder(graph, v, visited, order);
		}
	}

	// 2. Create transpose of the graph
	std::vector<std::vector<int>> transpose(n);
	const auto& adj = graph.getAdjList();
	for (int v = 0; v < n; ++v) 
    {
		for (int u : adj[v]) 
        {
			transpose[u].push_back(v);
		}
	}

	// 3. Process all vertices in order defined by Stack
	std::fill(visited.begin(), visited.end(), false);
	std::vector<std::vector<int>> sccs;
	while (!order.empty()) 
    {
		int v = order.top(); order.pop();
		if (!visited[v]) 
        {
			std::vector<int> component; // To store current SCC
			dfsOnTranspose(transpose, v, visited, component); // Get one SCC
			sccs.push_back(component); // Add current SCC to the list of SCCs
		} 
	}
	return sccs;
}
