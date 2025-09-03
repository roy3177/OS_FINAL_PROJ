#pragma once
#include "../../part_1/graph_impl.hpp"
#include <random>
#include <set>

// Directed/undirected random graph generator with integer weights in [wmin,wmax]
Graph generate_random_graph(int vertices, int edges, int seed, bool directed, int wmin, int wmax);
