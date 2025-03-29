test:test.cc
	g++ -o test test.cc -std=c++17 -pthread

.PHONY: clean
clean:
	rm -f test