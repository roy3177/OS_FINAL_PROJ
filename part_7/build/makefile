CXX=g++
CXXFLAGS=-std=c++17 -Wall -Wextra

# Rooted paths (inside build/)
ROOT=..
APPS=$(ROOT)/apps
ALGO_DIR=$(ROOT)/algorithms
SF=$(ROOT)/strategy_factory
PART1=$(ROOT)/../part_1

INCLUDES=-I$(ROOT) -I$(APPS) -I$(ALGO_DIR) -I$(SF) -I$(PART1)
# $(wildcard $(ALGO_DIR)/*.cpp) is used to auto-collect all algorithm .cpp files in algorithms/ so maintaining a manual list is not necessary.
ALGO_SRCS=$(wildcard $(ALGO_DIR)/*.cpp)

SRCS_ALGO=$(APPS)/algos_demo.cpp $(ALGO_SRCS) $(PART1)/graph_impl.cpp
SRCS_SERVER=$(APPS)/server.cpp $(SF)/AlgorithmFactory.cpp $(ALGO_SRCS) $(PART1)/graph_impl.cpp
SRCS_CLIENT=$(APPS)/client.cpp

all: main server client

main: $(SRCS_ALGO)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o main $(SRCS_ALGO)

server: $(SRCS_SERVER)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o server $(SRCS_SERVER)

client: $(SRCS_CLIENT)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o client $(SRCS_CLIENT)

clean:
	rm -f main server client *.o

.PHONY: all clean