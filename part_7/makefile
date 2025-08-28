CXX=g++
CXXFLAGS=-std=c++17 -Wall -Wextra

SRCS_ALGO=graph_algos.cpp Finding_Max_Flow.cpp Finding_Num_Cliques.cpp Finding_SCC.cpp MST_Weight.cpp ../part_1/graph_impl.cpp
SRCS_SERVER=server.cpp AlgorithmFactory.cpp Finding_Max_Flow.cpp Finding_Num_Cliques.cpp Finding_SCC.cpp MST_Weight.cpp ../part_1/graph_impl.cpp
SRCS_CLIENT=client.cpp

all: main server client

main: $(SRCS_ALGO)
	$(CXX) $(CXXFLAGS) -o main $(SRCS_ALGO)

server: $(SRCS_SERVER)
	$(CXX) $(CXXFLAGS) -o server $(SRCS_SERVER)

client: $(SRCS_CLIENT)
	$(CXX) $(CXXFLAGS) -o client $(SRCS_CLIENT)

clean:
	rm -f main server client *.o

.PHONY: all clean