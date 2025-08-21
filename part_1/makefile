CXX=g++
CXXFLAGS = -std=c++17 -Wall -Wextra


all: main

main: main.cpp graph_impl.hpp graph_impl.cpp
	$(CXX) $(CXXFLAGS)  -o main  main.cpp graph_impl.cpp -o main
clean:
	rm -f main
.PHONY: all clean