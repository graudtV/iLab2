ESSENTIAL_HEADERS=include/matrix.h include/matrix_impl.h include/type_conversions.h
ESSENTIAL_OBJS=obj/matrix.o
ESSENTIAL_FILES=$(ESSENTIAL_HEADERS) $(ESSENTIAL_OBJS)
OTHER_FLAGS=-std=c++17 -I include

.PHONY: all init clean compile-essential-objs io-tests

all: init compile-essential-objs io-tests

init:
	mkdir -p obj bin obj/examples bin/examples obj/test bin/test

clean:
	rm -rf obj bin

io-tests: bin/test/determinant
	$(call dir-cmp-outputs,./bin/test/determinant,test/data,.input,.output)

compile-essential-objs: $(ESSENTIAL_FILES)

bin/%: obj/%.o $(ESSENTIAL_FILES)
	$(CXX) $(OTHER_FLAGS) $(CXXFLAGS) $< $(ESSENTIAL_OBJS) -o $@

obj/%.o: src/%.cpp $(ESSENTIAL_HEADERS)
	$(CXX) $(OTHER_FLAGS) $(CXXFLAGS) -c $< -o $@

obj/%.o: %.cpp $(ESSENTIAL_HEADERS)
	$(CXX) $(OTHER_FLAGS) $(CXXFLAGS) -c $< -o $@

# $(1) - progname, $(2) - file, with which stdout of the programm will be compared
define cmp-output
$(1) | diff - $(2) -bB --brief
endef

# $(1) - progname, $(2) - data path, $(3) - input postfix, $(4) - output postfix 
# Inputs files are passed to programm stdin (not as params!)
define dir-cmp-outputs
 @for file in $(2)/*$(3);\
  do $(call echo-and-exec,$(call cmp-output,$(1) < $$file,$${file%*$(3)}$(4))); done
endef

#  Usefull inside bash for loop. Prints command in every iteration
# of the loop in form in which it exactly runs
#  Example: @for i in 1 2 3; do $(call echo-and-exec,ls); done
define echo-and-exec
echo "$(1)"; $(1)
endef
