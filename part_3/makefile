CXX=g++
CXXFLAGS = -std=c++17 -Wall -Wextra


all: main

main: main.cpp random_graph.hpp random_graph.cpp ../part_1/graph_impl.cpp ../part_2/euler_circle.cpp
	$(CXX) $(CXXFLAGS)  -o main  main.cpp random_graph.cpp ../part_1/graph_impl.cpp ../part_2/euler_circle.cpp
clean:
	rm -f main
.PHONY: all clean