.PHONY: all clean run-example test
CFLAGS=-std=c++17
GEOMETRY_HEADERS=geometry.h
GEOMETRY_OBJS=obj/geometry.o
GEOMETRY_FILES=$(GEOMETRY_HEADERS) $(GEOMETRY_OBJS)

all: init run-example

init:
	mkdir -p obj bin

run-example: bin/example
	./bin/example

test: bin/unit_test
	bin/unit_test

clean:
	rm -rf obj bin

bin/example: example.cpp $(GEOMETRY_FILES)
	$(CXX) $(CFLAGS) $< $(GEOMETRY_OBJS) -o $@

bin/unit_test: unit_test.cpp $(GEOMETRY_FILES)
	$(CXX) $(CFLAGS) $< $(GEOMETRY_OBJS) -o $@

obj/geometry.o: geometry.cpp geometry.h
	$(CXX) $(CFLAGS) -c $< -o $@
