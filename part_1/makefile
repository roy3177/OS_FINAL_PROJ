CXX=g++
CXXFLAGS = -std=c++17 -Wall -Wextra --coverage

all: main_case1 main_case2

main_case1: main_case1.cpp graph_impl.cpp
	$(CXX) $(CXXFLAGS) -o main_case1 main_case1.cpp graph_impl.cpp

main_case2: main_case2.cpp graph_impl.cpp
	$(CXX) $(CXXFLAGS) -o main_case2 main_case2.cpp graph_impl.cpp

gcov: all
	./main_case1
	./main_case2
	gcov main_case1-main_case1.gcno
	gcov main_case2-main_case2.gcno
	gcov main_case1-graph_impl.gcno
	gcov main_case2-graph_impl.gcno

clean:
	rm -f main_case1 main_case2 *.gcno *.gcda *.gcov

.PHONY: all clean gcov