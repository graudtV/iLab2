GEOMETRY_HEADERS=geometry.h geometry_intersections_impl.h other.h
GEOMETRY_OBJS=obj/geometry.o obj/geometry_intersections_impl.o
GEOMETRY_FILES=$(GEOMETRY_HEADERS) $(GEOMETRY_OBJS)
CONST_FLAGS=-std=c++17

.SECONDARY: $(GEOMETRY_OBJS)
.PHONY: all clean run-example test

all: init test

init:
	mkdir -p obj bin

run-example: bin/example
	./bin/example

test: bin/unit_test bin/test_nintersections_trg_trg test/data/generated
	bin/unit_test
	$(call run-test,test/data/1.input,test/data/1.output)
	$(call run-test,test/data/2.input,test/data/2.output)
	$(call run-test,test/data/3.input,test/data/3.output)
	$(call run-test,test/data/generated/10.input,test/data/generated/10.output)
	$(call run-test,test/data/generated/100.input,test/data/generated/100.output)
	$(call run-test,test/data/generated/2000.input,test/data/generated/2000.output)

test/data/generated: bin/trggen
	mkdir -p test/data/generated
	$(call generate-test-data,10)
	$(call generate-test-data,100)
	$(call generate-test-data,1000)
	$(call generate-test-data,2000)


define generate-test-data
	./bin/trggen $(1) > 'test/data/generated/$(1).input'
	bin/test_nintersections_trg_trg -b < 'test/data/generated/$(1).input' > 'test/data/generated/$(1).output'
endef

define run-test
	bin/test_nintersections_trg_trg < $(1) | diff - $(2) --brief
endef

clean:
	rm -rf obj bin test/data/generated

bin/%: test/%.cpp $(GEOMETRY_FILES)
	$(CXX) $(CONST_FLAGS) $(CXXFLAGS) $< $(GEOMETRY_OBJS) -o $@

obj/%.o: %.cpp $(GEOMETRY_HEADERS)
	$(CXX) $(CONST_FLAGS) $(CXXFLAGS) -c $< -o $@
