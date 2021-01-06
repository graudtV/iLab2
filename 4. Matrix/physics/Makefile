ESSENTIAL_HEADERS=include/*
EXTRA_FLAGS=-I include -I ../maths/include -I scan/ -std=c++17

include ../../basic_tests.mk

#$(call create-io-tests,calc-currents-test,bin/test/calc_currents,test/data/currents)

all: compile-essential-objs bin/test/calc_currents $(calc-currents-test)

scan/scan.h:
	git submodule init && git submodule update

include ../../basic_compilation.mk
