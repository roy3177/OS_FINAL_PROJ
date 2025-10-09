#pragma once

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
#include <algorithm>
#include <cstring>
#include <iostream>
#include <limits>
#include <set>
#include <sstream>
#include <string>
#include <tuple>
#include <poll.h>
#include <cstdlib>

#define PORT 9090

// Helper UI/IO utilities for the client
void println_rule();
int prompt_int(const std::string& msg, int minVal, int maxVal);
int run_client(int argc, char* argv[]);
bool getline_with_timeout(std::string &out, int timeout_ms);
