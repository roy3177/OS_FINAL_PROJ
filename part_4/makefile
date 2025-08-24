CXX=g++
CXXFLAGS = -std=c++17 -Wall -Wextra --coverage


all: main

main: main.cpp ../part_1/graph_impl.cpp ../part_2/euler_circle.cpp ../part_3/random_graph.cpp
	$(CXX) $(CXXFLAGS)  -o main  main.cpp ../part_1/graph_impl.cpp ../part_2/euler_circle.cpp ../part_3/random_graph.cpp
gcov: all
	./run_tests.sh
	gcov main-main.gcno
valgrind: main
	valgrind --leak-check=full ./main -v $(shell echo $$((RANDOM % 50 + 2))) -e $(shell echo $$((RANDOM % 100 + 1))) -s $(shell date +%s)
clean:
	rm -f main *.gcno *.gcda *.gcov
.PHONY: all clean