.PHONY: all clean example run-example test
CC = cc
CFLAGS = -std=c++11 -lc++
HEADERS = include/cache.h include/cache_realization.h include/database.h

all:
example: bin/example
run-example: bin/example
	bin/example
clean:
	rm -rf bin

test: bin/test bin/test/test_lru bin/test/test_efficiency
	bin/test/test_lru
	bin/test/test_efficiency

bin/example: src/example.cpp $(HEADERS) bin
	$(CC) $(CFLAGS) -o $@ $<

bin/test/test_lru: test/test_lru.cpp $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $<
bin/test/test_efficiency: test/test_efficiency.cpp $(HEADERS) test/graph.h test/graph_realization.h
	$(CC) $(CFLAGS) -o $@ $<

bin:
	mkdir bin
bin/test: bin
	mkdir bin/test
