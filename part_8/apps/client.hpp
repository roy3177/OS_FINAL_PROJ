#pragma once
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <algorithm>
#include <cstring>
#include <iostream>
#include <limits>
#include <set>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>
#include <cstdlib>
#include <cctype>

#ifndef PORT
#define PORT 9090
#endif

void println_rule();
int prompt_int(const std::string& msg, int minVal, int maxVal);
int run_client(int argc, char* argv[]);
