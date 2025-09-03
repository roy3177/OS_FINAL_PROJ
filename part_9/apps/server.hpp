#pragma once

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <cstdlib>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>

#include "../../part_1/graph_impl.hpp"
// Reuse Part 8's random graph interface; implementation will be linked via makefile sources.
#include "../../part_8/include/random_graph.hpp"
#include "../../part_7/strategy_factory/AlgorithmFactory.hpp"

#ifndef PORT
#define PORT 9090
#endif

bool recv_all_lines(int fd, std::string &out);
void send_response(int fd, const std::string &body, bool ok = true);

// Leader–Follower API
int run_server(int argc, char* argv[]);
void lf_server_loop(int srv_fd, int workers);
void handle_client(int fd);
