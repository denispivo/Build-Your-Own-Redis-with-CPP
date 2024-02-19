CXX       := /usr/bin/g++
CXX_FLAGS := -std=c++17 -Wall -Wextra -O2 -g
#-Wpedantic -fdiagnostics-color=always

all: client.exe server.exe

client.exe: client.cpp
	$(CXX) $(CXX_FLAGS) $^ -o $@

server.exe: server.cpp
	$(CXX) $(CXX_FLAGS) $^ -o $@

clean:
	rm -f *.exe
