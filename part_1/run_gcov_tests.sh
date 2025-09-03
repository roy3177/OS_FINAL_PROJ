#!/bin/bash

make clean

# Test case 1: default graph
cp main_case1.cpp main.cpp
make all
./main

# Test case 2: different graph
cp main_case2.cpp main.cpp
make all
./main



# Generate coverage reports
gcov main-main.gcno
gcov main-graph_impl.gcno