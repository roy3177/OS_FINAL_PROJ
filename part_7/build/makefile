CXX       = g++
CXXFLAGS  = -std=c++17 -Wall -Wextra -O0 -g --coverage -fprofile-arcs -ftest-coverage
LDFLAGS   = --coverage

ROOT      = ..
APPS      = $(ROOT)/apps
ALGO_DIR  = $(ROOT)/algorithms
SF        = $(ROOT)/strategy_factory
PART1     = $(ROOT)/../part_1

INCLUDES  = -I$(ROOT) -I$(APPS) -I$(ALGO_DIR) -I$(SF) -I$(PART1)
ALGO_SRCS = $(wildcard $(ALGO_DIR)/*.cpp)

SRCS_ALGO   = $(APPS)/algos_demo.cpp $(ALGO_SRCS) $(PART1)/graph_impl.cpp
SRCS_SERVER = $(APPS)/server.cpp     $(SF)/AlgorithmFactory.cpp $(ALGO_SRCS) $(PART1)/graph_impl.cpp
SRCS_CLIENT = $(APPS)/client.cpp

OBJS_SERVER = server.o AlgorithmFactory.o $(patsubst $(ALGO_DIR)/%.cpp,%.o,$(ALGO_SRCS)) graph_impl.o
OBJS_CLIENT = client.o
OBJS_ALGO   = algos_demo.o $(patsubst $(ALGO_DIR)/%.cpp,%.o,$(ALGO_SRCS)) graph_impl.o

BIN_MAIN   = main
BIN_SERVER = server
BIN_CLIENT = client

.PHONY: all clean gcov valgrind memcheck callgrind helgrind

# ===== Build =====
all: $(BIN_MAIN) $(BIN_SERVER) $(BIN_CLIENT)

%.o: $(ALGO_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

AlgorithmFactory.o: $(SF)/AlgorithmFactory.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

graph_impl.o: $(PART1)/graph_impl.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

server.o: $(APPS)/server.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

client.o: $(APPS)/client.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

algos_demo.o: $(APPS)/algos_demo.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BIN_SERVER): $(OBJS_SERVER)
	$(CXX) $^ $(LDFLAGS) -o $@

$(BIN_CLIENT): $(OBJS_CLIENT)
	$(CXX) $^ $(LDFLAGS) -o $@

$(BIN_MAIN): $(OBJS_ALGO)
	$(CXX) $^ $(LDFLAGS) -o $@

# ===== GCOV =====
gcov: $(BIN_SERVER) $(BIN_CLIENT)
	./run_tests.sh
	@echo "=== GCOV REPORTS ==="
	@echo "--- Client ---"
	gcov -b -c $(APPS)/client.cpp -o . || true
	@echo "--- Server ---"
	gcov -b -c $(APPS)/server.cpp -o . || true

# ===== Valgrind =====
valgrind: memcheck

memcheck: $(BIN_SERVER) $(BIN_CLIENT)
	@echo "== Valgrind Memcheck =="
	./$(BIN_SERVER) > /dev/null 2>&1 &
	sleep 0.3
	printf "0\n" | valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(BIN_CLIENT)

callgrind: $(BIN_SERVER) $(BIN_CLIENT)
	@echo "== Valgrind Callgrind =="
	./$(BIN_SERVER) > /dev/null 2>&1 &
	sleep 0.3
	printf "0\n" | valgrind --tool=callgrind --callgrind-out-file=callgrind.out ./$(BIN_CLIENT)
	@echo "Use kcachegrind to view: callgrind.out"

helgrind: $(BIN_SERVER) $(BIN_CLIENT)
	@echo "== Valgrind Helgrind =="
	./$(BIN_SERVER) > /dev/null 2>&1 &
	sleep 0.3
	printf "0\n" | valgrind --tool=helgrind ./$(BIN_CLIENT)

# ===== Clean =====
clean:
	rm -f $(BIN_MAIN) $(BIN_SERVER) $(BIN_CLIENT) *.o \
	      *.gcno *.gcda *.gcov \
	      callgrind.out* cachegrind.out* gmon.out \
	      $(APPS)/*.gcno $(APPS)/*.gcda $(APPS)/*.gcov \
	      $(ALGO_DIR)/*.gcno $(ALGO_DIR)/*.gcda $(ALGO_DIR)/*.gcov \
	      $(SF)/*.gcno $(SF)/*.gcda $(SF)/*.gcov \
	      $(PART1)/*.gcno $(PART1)/*.gcda $(PART1)/*.gcov
	rm -rf build
