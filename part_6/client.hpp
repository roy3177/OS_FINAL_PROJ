#pragma once

#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <limits>
#include <algorithm>
#include <set>

#define PORT 8080 // Default port

void run_client();
