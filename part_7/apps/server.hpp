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

#include "Finding_Max_Flow.hpp"
#include "../part_1/graph_impl.hpp"
#include "../strategy_factory/AlgorithmFactory.hpp"

#define PORT 9090 // Default port

bool recv_all_lines(int fd, std::string &out);
void send_response(int fd, const std::string &body, bool ok = true);
int run_server(int argc, char* argv[]);
