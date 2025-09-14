#!/bin/bash

# Test: insufficient arguments(default is zero in all arguments)
./main

# Test: argument usage is invalid
./main -v 4 -e 4 -z 42

# Test: valid graph with Eulerian circuit
./main -v 4 -e 4 -s 4

# Test: valid graph without Eulerian circuit
./main -v 4 -e 4 -s 42