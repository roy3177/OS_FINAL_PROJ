CXX=g++
CXXFLAGS=-std=c++17 -Wall -Wextra

SRCS_ALGO=graph_algos.cpp Finding_Max_Flow.cpp Finding_Num_Cliques.cpp ../part_1/graph_impl.cpp

all: main

main: $(SRCS_ALGO)
	$(CXX) $(CXXFLAGS) -o main $(SRCS_ALGO)

clean:
	rm -f main *.o

.PHONY: all clean