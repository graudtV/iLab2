#---------------------------------------------------------
#  This file is intended to be included in other
# makefiles to make compilation and linkage simplier
#  File should be included in the end of parent makefile
#---------------------------------------------------------
# author: https://github.com/graudtV
#---------------------------------------------------------

#  These variables can be set before including this makefile to
# maintain compilation
ESSENTIAL_HEADERS+=
ESSENTIAL_OBJS+=
ESSENTIAL_FILES+=$(ESSENTIAL_HEADERS) $(ESSENTIAL_OBJS)
EXTRA_FLAGS+=

.PHONY:all clean compile-essential-objs unit-tests
.DEFAULT_GOAL=all

clean:
	rm -rf obj bin

compile-essential-objs: $(ESSENTIAL_OBJS)

bin/%: obj/%.o $(ESSENTIAL_OBJS)
	$(call create-dir-if-not-exist,$(dir  $@))
	$(CXX) $(EXTRA_FLAGS) $(CXXFLAGS) $< $(ESSENTIAL_OBJS) -o $@

obj/%.o: src/%.cpp $(ESSENTIAL_HEADERS)
	$(call create-dir-if-not-exist,$(dir  $@))
	$(CXX) $(EXTRA_FLAGS) $(CXXFLAGS) -c $< -o $@

obj/%.o: %.cpp $(ESSENTIAL_HEADERS)
	$(call create-dir-if-not-exist,$(dir  $@))
	$(CXX) $(EXTRA_FLAGS) $(CXXFLAGS) -c $< -o $@

# $(1) - directory name
define create-dir-if-not-exist
@[ -d $(1) ] || $(call echo-and-exec,mkdir -p $(1))
endef

# Usefull inside bash for loop or other features connected with bash
# Prints command in every iteration of the loop in form in which it exactly runs
# Example: @for i in 1 2 3; do $(call echo-and-exec,ls); done
define echo-and-exec
(echo "$(1)"; $(1))
endef
