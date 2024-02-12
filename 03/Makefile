CXX       := /usr/bin/g++
CXX_FLAGS := -std=c++17 -Wall -Wextra -Wpedantic -fdiagnostics-color=always -O2 -g

main.exe: 03_client.cpp 03_server.cpp
	$(CXX) $(CXX_FLAGS) $^ -o $@

run: ./03_server.exe ./03_client.exe

clean:
	rm -f *.exe
