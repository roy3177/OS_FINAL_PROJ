CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O0 -g -fprofile-arcs -ftest-coverage
LDFLAGS  = -fprofile-arcs -ftest-coverage

all: main_case1 main_case2

main_case1: main_case1.o graph_impl.o
	$(CXX) $(LDFLAGS) -o $@ $^

main_case2: main_case2.o graph_impl.o
	$(CXX) $(LDFLAGS) -o $@ $^

main_case1.o: main_case1.cpp graph_impl.hpp
	$(CXX) $(CXXFLAGS) -c $<

main_case2.o: main_case2.cpp graph_impl.hpp
	$(CXX) $(CXXFLAGS) -c $<

graph_impl.o: graph_impl.cpp graph_impl.hpp
	$(CXX) $(CXXFLAGS) -c $<

gcov: all
	./main_case1
	./main_case2
	# generate gcov reports
	gcov -b -c -o . graph_impl.cpp main_case1.cpp main_case2.cpp

valgrind-mem: all
	valgrind --leak-check=full --show-leak-kinds=all ./main_case1
	valgrind --leak-check=full --show-leak-kinds=all ./main_case2

valgrind-hel: all
	valgrind --tool=helgrind ./main_case1
	valgrind --tool=helgrind ./main_case2

valgrind-cg: all
	valgrind --tool=callgrind ./main_case1
	valgrind --tool=callgrind ./main_case2


clean:
	rm -f main_case1 main_case2 *.o *.gcno *.gcda *.gcov

.PHONY: all clean gcov
