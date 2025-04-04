
.PHONY: all
all: parse server

parse: Parse.cc
	g++ -o parse Parse.cc -std=c++17 -pthread -ljsoncpp

server: Server.cc
	g++ -o server Server.cc -std=c++17 -pthread -ljsoncpp

.PHONY: clean
clean:
	rm -f server parse