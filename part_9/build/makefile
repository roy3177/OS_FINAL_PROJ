CXX=g++
CXXFLAGS=-std=c++17 -Wall -Wextra -pthread -O0 -g -fprofile-arcs -ftest-coverage
LDFLAGS=--coverage

# Paths
ROOT=..
PART1=$(ROOT)/../part_1
PART7=$(ROOT)/../part_7
APPS=$(ROOT)/apps
INC=$(ROOT)/include

INCLUDES=-I$(ROOT) -I$(APPS) -I$(INC) -I$(PART1) -I$(PART7) -I$(PART7)/algorithms -I$(PART7)/strategy_factory
ALGO_SRCS=$(wildcard $(PART7)/algorithms/*.cpp)
ALGO_OBJS=$(patsubst %.cpp,%.o,$(notdir $(ALGO_SRCS)))
RAND_SRC=$(ROOT)/../part_8/include/random_graph.cpp

BIN_SERVER=server
BIN_CLIENT=client

.PHONY: all clean gcov valgrind memcheck callgrind helgrind

# ===== Build =====
all: $(BIN_SERVER) $(BIN_CLIENT)

server.o: $(APPS)/server.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

AlgorithmFactory.o: $(PART7)/strategy_factory/AlgorithmFactory.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

graph_impl.o: $(PART1)/graph_impl.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

random_graph.o: $(RAND_SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(ALGO_OBJS): %.o: $(PART7)/algorithms/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# link
$(BIN_SERVER): server.o AlgorithmFactory.o graph_impl.o random_graph.o $(ALGO_OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

$(BIN_CLIENT): $(APPS)/client.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LDFLAGS) -o $@ $^

gcov: $(BIN_SERVER) $(BIN_CLIENT)
	./run_tests.sh
	@echo "=== GCOV REPORTS ==="
	@echo "--- Server ---";  gcov -b -c $(APPS)/server.cpp  -o . | grep -E "File|Lines executed|Branches executed|Taken at least once" || true
	@echo "--- Client ---";  gcov -b -c $(APPS)/client.cpp  -o . | grep -E "File|Lines executed|Branches executed|Taken at least once" || true

# ===== Clean =====
clean:
	rm -f $(BIN_SERVER) $(BIN_CLIENT) *.o \
	      *.gcno *.gcda *.gcov \
	      callgrind.out* cachegrind.out* gmon.out
