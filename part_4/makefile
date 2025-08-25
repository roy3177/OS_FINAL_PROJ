CXX=g++
CXXFLAGS = -std=c++17 -Wall -Wextra --coverage -pg


all: main

main: main.cpp ../part_1/graph_impl.cpp ../part_2/euler_circle.cpp ../part_3/random_graph.cpp
	$(CXX) $(CXXFLAGS)  -o main  main.cpp ../part_1/graph_impl.cpp ../part_2/euler_circle.cpp ../part_3/random_graph.cpp
gcov: all
	./run_tests.sh
	gcov main-main.gcno
# $(shell echo $$((RANDOM % 50 + 2))) picks a random number of vertices between 2 and 51.
# $(shell echo $$((RANDOM % 100 + 1))) picks a random number of edges between 1 and 100.
# $(shell date +%s) gives a random seed.
valgrind: main
	valgrind --leak-check=full ./main -v $(shell echo $$((RANDOM % 50 + 2))) -e $(shell echo $$((RANDOM % 100 + 1))) -s $(shell date +%s)
callgraph: main
	valgrind --tool=callgrind ./main -v 10 -e 15 -s 42
gprof: main
	./main -v 10 -e 15 -s 42
	gprof ./main gmon.out > gprof_report.txt
clean:
	rm -f main *.gcno *.gcda *.gcov callgrind.out* gprof_report.txt gmon.out
.PHONY: all clean