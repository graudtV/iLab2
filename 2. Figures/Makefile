GEOMETRY_HEADERS=geometry.h geometry_intersections_impl.h other.h
GEOMETRY_OBJS=obj/geometry.o obj/geometry_intersections_impl.o
GEOMETRY_FILES=$(GEOMETRY_HEADERS) $(GEOMETRY_OBJS)
OTHER_FLAGS=-std=c++17

# Folder, where auto-generated data-files for tests will be placed
DATA_FILES_FOR_TESTS_PATH=test/data/generated

# Number of auto-generated data-files for tests. Tests run automatically
N_TRGS_TO_GENERATE_DENSE=100 1000
N_TRGS_TO_GENERATE_SPARSE=100 1000 2000

.SECONDARY: $(GEOMETRY_OBJS) bin/trggen
.PHONY: all clean run-example test unit-tests other-tests

all: init unit-tests other-tests

init:
	mkdir -p obj bin $(DATA_FILES_FOR_TESTS_PATH)/sparse $(DATA_FILES_FOR_TESTS_PATH)/dense

run-example: example.cpp $(GEOMETRY_FILES)
	$(CXX) $(OTHER_FLAGS) $(CXXFLAGS) $< $(GEOMETRY_OBJS) -o bin/example
	./bin/example

clean:
	rm -rf obj bin $(DATA_FILES_FOR_TESTS_PATH)

bin/%: test/%.cpp $(GEOMETRY_FILES)
	$(CXX) $(OTHER_FLAGS) $(CXXFLAGS) $< $(GEOMETRY_OBJS) -o $@

obj/%.o: %.cpp $(GEOMETRY_HEADERS)
	$(CXX) $(OTHER_FLAGS) $(CXXFLAGS) -c $< -o $@



# TESTS building and running
DATA_FILES_FOR_TESTS_BASENAME=$(addprefix $(DATA_FILES_FOR_TESTS_PATH)/dense/,$(N_TRGS_TO_GENERATE_DENSE))\
 $(addprefix $(DATA_FILES_FOR_TESTS_PATH)/sparse/,$(N_TRGS_TO_GENERATE_SPARSE))
DATA_FILES_FOR_TESTS=\
 $(addsuffix .input,$(DATA_FILES_FOR_TESTS_BASENAME))\
 $(addsuffix .output,$(DATA_FILES_FOR_TESTS_BASENAME))\
 $(addsuffix .indices.output,$(DATA_FILES_FOR_TESTS_BASENAME))

test: unit-tests other-tests

unit-tests: bin/unit_test
	bin/unit_test

other-tests: bin/test_nintersections_trg_trg $(DATA_FILES_FOR_TESTS)
	$(call check-ntrgs,test/data/1.input,test/data/1.output)
	$(call check-ntrgs,test/data/2.input,test/data/2.output)
	$(call check-ntrgs,test/data/3.input,test/data/3.output)
	$(call check-ntrgs,test/data/4.input,test/data/4.output)
	$(call check-indices,test/data/1.input,test/data/1.indices.output)
	$(call check-indices,test/data/2.input,test/data/2.indices.output)
	$(call check-indices,test/data/3.input,test/data/3.indices.output)
	$(call check-indices,test/data/4.input,test/data/4.indices.output)
	@for file_basename in $(DATA_FILES_FOR_TESTS_BASENAME); do\
	 $(call echo-and-exec,$(call check-ntrgs,$$file_basename.input,$$file_basename.output)); done
	@for file_basename in $(DATA_FILES_FOR_TESTS_BASENAME); do\
	 $(call echo-and-exec,$(call check-indices,$$file_basename.input,$$file_basename.indices.output)); done

$(DATA_FILES_FOR_TESTS_PATH)/dense/%.input: bin/trggen
	bin/trggen $(notdir $(basename $@)) > $@

$(DATA_FILES_FOR_TESTS_PATH)/sparse/%.input: bin/trggen
	bin/trggen -s $(notdir $(basename $@)) > $@

$(DATA_FILES_FOR_TESTS_PATH)/%.output: $(DATA_FILES_FOR_TESTS_PATH)/%.input
	bin/test_nintersections_trg_trg -nb < $< > $@

$(DATA_FILES_FOR_TESTS_PATH)/%.indices.output: $(DATA_FILES_FOR_TESTS_PATH)/%.input
	bin/test_nintersections_trg_trg -ib < $< > $@

define check-ntrgs
bin/test_nintersections_trg_trg -n < $(1) | diff - $(2) -bB --brief
endef

define check-indices
bin/test_nintersections_trg_trg -i < $(1) | diff - $(2) -bB --brief
endef

#  Usefull inside bash for loop. Prints command in every iteration
# of the loop in form in which it exactly runs
#  Example: @for i in 1 2 3; do $(call echo-and-exec,ls); done
define echo-and-exec
echo "$(1)"; $(1)
endef
