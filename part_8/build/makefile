CXX=g++
CXXFLAGS=-std=c++17 -Wall -Wextra -pthread -O0 -g --coverage -fprofile-arcs -ftest-coverage
LDFLAGS=--coverage

# Paths
ROOT=..
PART1=$(ROOT)/../part_1
PART7=$(ROOT)/../part_7
APPS=$(ROOT)/apps
INC=$(ROOT)/include

INCLUDES=-I$(ROOT) -I$(APPS) -I$(INC) -I$(PART1) -I$(PART7) -I$(PART7)/algorithms -I$(PART7)/strategy_factory
ALGO_SRCS=$(wildcard $(PART7)/algorithms/*.cpp)

SERVER_SRCS=$(APPS)/server.cpp $(INC)/random_graph.cpp $(PART7)/strategy_factory/AlgorithmFactory.cpp $(ALGO_SRCS) $(PART1)/graph_impl.cpp
CLIENT_SRCS=$(APPS)/client.cpp

BIN_SERVER=server
BIN_CLIENT=client

.PHONY: all clean valgrind memcheck callgrind helgrind

# ===== Build =====
all: $(BIN_SERVER) $(BIN_CLIENT)

server.o: $(APPS)/server.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

random_graph.o: $(INC)/random_graph.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

AlgorithmFactory.o: $(PART7)/strategy_factory/AlgorithmFactory.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

graph_impl.o: $(PART1)/graph_impl.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BIN_SERVER): server.o random_graph.o AlgorithmFactory.o graph_impl.o
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LDFLAGS) -o $@ \
	    server.o random_graph.o AlgorithmFactory.o graph_impl.o \
	    $(ALGO_SRCS)


$(BIN_CLIENT): $(CLIENT_SRCS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LDFLAGS) -o $@ $^

# ===== Valgrind =====

# ===== GCOV =====
gcov: $(BIN_SERVER) $(BIN_CLIENT)
	./run_tests.sh
	@echo "=== GCOV REPORTS ==="
	@echo "--- Client ---"
	gcov -b -c $(APPS)/client.cpp -o . || true
	@echo "--- Server ---"
	gcov -b -c $(APPS)/server.cpp -o . || true
	@echo "--- Random Graph ---"
	gcov -b -c $(INC)/random_graph.cpp -o . || true
	
valgrind: valgrind-mem

valgrind-mem: $(BIN_SERVER) $(BIN_CLIENT)
	@echo "== Valgrind Memcheck =="
	./$(BIN_SERVER) > /dev/null 2>&1 &
	sleep 0.3
	printf "0\n" | valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(BIN_CLIENT)

valgrind-cg: $(BIN_SERVER) $(BIN_CLIENT)
	@echo "== Valgrind Callgrind =="
	./$(BIN_SERVER) > /dev/null 2>&1 &
	sleep 0.3
	printf "0\n" | valgrind --tool=callgrind --callgrind-out-file=callgrind.out ./$(BIN_CLIENT)
	@echo "Use kcachegrind to view: callgrind.out"

valgrind-hel: $(BIN_SERVER) $(BIN_CLIENT)
	@echo "== Valgrind Helgrind =="
	./$(BIN_SERVER) > /dev/null 2>&1 &
	sleep 0.3
	printf "0\n" | valgrind --tool=helgrind ./$(BIN_CLIENT)

# ===== Clean =====
clean:
	rm -f $(BIN_SERVER) $(BIN_CLIENT) *.o \
		  *.gcno *.gcda *.gcov \
		  callgrind.out* cachegrind.out* gmon.out