CXX=g++
CXXFLAGS=-std=c++17 -Wall -Wextra -pthread

# Paths
ROOT=..
PART1=$(ROOT)/../part_1
PART7=$(ROOT)/../part_7
APPS=$(ROOT)/apps
INC=$(ROOT)/include

INCLUDES=-I$(ROOT) -I$(APPS) -I$(INC) -I$(PART1) -I$(PART7) -I$(PART7)/algorithms -I$(PART7)/strategy_factory
ALGO_SRCS=$(wildcard $(PART7)/algorithms/*.cpp)

SERVER_SRCS=$(APPS)/server.cpp $(APPS)/random_graph.cpp $(PART7)/strategy_factory/AlgorithmFactory.cpp $(ALGO_SRCS) $(PART1)/graph_impl.cpp
CLIENT_SRCS=$(APPS)/client.cpp

all: server client

server: $(SERVER_SRCS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o server $(SERVER_SRCS)

client: $(CLIENT_SRCS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o client $(CLIENT_SRCS)

clean:
	rm -f server client *.o

.PHONY: all clean
