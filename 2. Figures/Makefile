GEOMETRY_HEADERS=geometry.h geometry_intersections_impl.h other.h
GEOMETRY_OBJS=obj/geometry.o obj/geometry_intersections_impl.o
GEOMETRY_FILES=$(GEOMETRY_HEADERS) $(GEOMETRY_OBJS)
OTHER_FLAGS=-std=c++17

# Folder, where auto-generated data-files for tests will be places
DATA_FILES_FOR_TESTS_PATH=test/data/generated/

# Number of auto-generated data-files for tests. Tests run automatically
N_TRGS_TO_GENERATE=10 100 1000

.SECONDARY: $(GEOMETRY_OBJS)
.PHONY: all clean run-example test

all: init test

init:
	mkdir -p obj bin $(DATA_FILES_FOR_TESTS_PATH)

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
DATA_FILES_FOR_TESTS=$(addprefix $(DATA_FILES_FOR_TESTS_PATH),\
	$(addsuffix .input, $(N_TRGS_TO_GENERATE)) \
	$(addsuffix .output, $(N_TRGS_TO_GENERATE)))

test: bin/unit_test bin/test_nintersections_trg_trg $(DATA_FILES_FOR_TESTS)
	bin/unit_test
	$(call compare-files,test/data/1.input,test/data/1.output)
	$(call compare-files,test/data/2.input,test/data/2.output)
	$(call compare-files,test/data/3.input,test/data/3.output)
	@for ntrgs in $(N_TRGS_TO_GENERATE);do\
	 $(call echo-and-exec,$(call compare-files,$(DATA_FILES_FOR_TESTS_PATH)$$ntrgs.input,\
	 $(DATA_FILES_FOR_TESTS_PATH)$$ntrgs.output)); done

$(DATA_FILES_FOR_TESTS_PATH)%.input: bin/trggen
	./bin/trggen $(notdir $(basename $@)) > $@

$(DATA_FILES_FOR_TESTS_PATH)%.output: $(DATA_FILES_FOR_TESTS_PATH)%.input
	bin/test_nintersections_trg_trg -nb < $< > $@

define compare-files
bin/test_nintersections_trg_trg -n < $(1) | diff - $(2) --brief
endef

#  Usefull inside bash for loop. Prints command in every iteration
# of the loop in form in which it exactly runs
#  Example: @for i in 1 2 3; do $(call echo-and-exec,ls); done
define echo-and-exec
echo "$(1)"; $(1)
endef
