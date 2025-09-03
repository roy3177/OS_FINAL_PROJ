/*
Pipeline types and minimal API used by the server.
This header defines the Job that flows through the pipeline and
declarations to start/stop the pipeline threads (implemented elsewhere).
*/

#pragma once

#include <string>
#include <unordered_map>

#include "../../part_1/graph_impl.hpp" // Graph type used inside Job

// What kind of request this Job represents.
enum class AlgKind 
{
	PREVIEW,        // Only preview the graph (no algorithms)
	ALL,            // Run all algorithms and aggregate
	SINGLE_MAX_FLOW,
	SINGLE_SCC,
	SINGLE_MST,
	SINGLE_CLIQUES
};

// A unit of work that flows through the pipeline stages.
// Ownership model: one stage owns a Job at a time (pop -> mutate -> push).
struct Job 
{
	// Connection
	int fd = -1;                 // client socket to reply on

	// Request metadata
	AlgKind kind = AlgKind::ALL; // what to compute
	bool directed = false;       // graph orientation requested by client

	// Inputs for computation
	Graph graph{0, false};       // the graph to operate on
	std::unordered_map<std::string,int> params; // SRC/SINK/K etc.

	// Results (filled by stages; string to keep exact messages like errors)
	std::string res_max_flow;    // either numeric string or "Error: …"
	std::string res_scc;         // SCC count or error
	std::string res_mst;         // MST weight or error
	std::string res_cliques;     // cliques count or error
};

// Pipeline lifecycle (to be implemented in server.cpp or a dedicated .cpp)
void start_pipeline();
void stop_pipeline();
