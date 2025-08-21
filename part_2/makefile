CXX=g++
CXXFLAGS = -std=c++17 -Wall -Wextra


all: main

main: main.cpp euler_circle.hpp euler_circle.cpp ../part_1/graph_impl.cpp
	$(CXX) $(CXXFLAGS)  -o main  main.cpp euler_circle.cpp ../part_1/graph_impl.cpp
clean:
	rm -f main
.PHONY: all clean