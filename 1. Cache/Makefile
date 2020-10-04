.PHONY: all clean example run-example test
CC = cc
CFLAGS = -std=c++11 -lc++
HEADERS = include/cache.h include/cache_realization.h include/database.h

all: example test

example: bin/example

run-example: example
	bin/example

clean:
	rm -rf bin

test: bin/test/test_efficiency

run-test: test
	bin/test/test_efficiency -gr 1000000 1000 10

bin/example: src/example.cpp $(HEADERS) bin
	$(CC) $(CFLAGS) -o $@ $<

bin/test/test_efficiency: test/test_efficiency.cpp test/testing_facilities.h $(HEADERS) bin/test
	$(CC) $(CFLAGS) -o $@ $<

bin:
	mkdir -p bin

bin/test:
	mkdir -p bin/test
