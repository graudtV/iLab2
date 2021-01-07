ESSENTIAL_HEADERS=include/*
ESSENTIAL_OBJS=obj/electronic_circuit.o
EXTRA_FLAGS=-I include -I ../maths/include -I scan/ -std=c++17

include ../../basic_tests.mk

$(call create-io-tests,incidence-matrix-test,bin/test/calc_currents -m,test/data/incidence_matrix)
$(call create-io-tests,voltage-srcs-matrix-test,bin/test/calc_currents -v,test/data/voltage_srcs_matrix)
$(call create-io-tests,current-srcs-matrix-test,bin/test/calc_currents -i,test/data/current_srcs_matrix)
#$(call create-io-tests,calc-currents-test,bin/test/calc_currents,test/data/currents)

all: compile-essential-objs bin/test/calc_currents $(incidence-matrix-test)\
	$(voltage-srcs-matrix-test) $(current-srcs-matrix-test) $(calc-currents-test)

scan/scan.h:
	git submodule init && git submodule update

include ../../basic_compilation.mk
