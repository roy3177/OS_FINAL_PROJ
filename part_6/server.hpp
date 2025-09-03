#pragma once

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <vector>
#include <sstream>
#include "../part_1/graph_impl.hpp"
#include "../part_2/euler_circle.hpp"

#define PORT 8080 // Default port

void run_server();
