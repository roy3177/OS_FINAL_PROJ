CXX=g++
CXXFLAGS=-std=c++17 -Wall -Wextra

SRCS_SERVER=server.cpp ../part_1/graph_impl.cpp ../part_2/euler_circle.cpp
SRCS_CLIENT=client.cpp

all: server client

server: $(SRCS_SERVER) server.hpp
	$(CXX) $(CXXFLAGS) -o server $(SRCS_SERVER)

client: $(SRCS_CLIENT) client.hpp
	$(CXX) $(CXXFLAGS) -o client $(SRCS_CLIENT)

clean:
	rm -f server client

.PHONY: all clean