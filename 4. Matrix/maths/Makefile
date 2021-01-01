ESSENTIAL_HEADERS=include/*
EXTRA_FLAGS=-I include -std=c++17

include ../../basic_tests.mk

$(call create-io-tests,determinant-tests,bin/test/determinant,test/data/determinant)
$(call add-unit-test-to-group,unit-tests,bin/test/unit_test)

all: compile-essential-objs bin/test/determinant $(determinant-tests) $(unit-tests)

include ../../basic_compilation.mk