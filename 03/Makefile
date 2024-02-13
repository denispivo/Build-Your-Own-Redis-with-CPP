CXX       := /usr/bin/g++
CXX_FLAGS := -std=c++17 -Wall -Wextra -Wpedantic -fdiagnostics-color=always -O2 -g

all: client.exe server.exe

client.exe: 03_client.cpp
	$(CXX) $(CXX_FLAGS) $^ -o $@

server.exe: 03_server.cpp
	$(CXX) $(CXX_FLAGS) $^ -o $@

clean:
	rm -f *.exe
