ESSENTIAL_HEADERS=include/*
ESSENTIAL_OBJS=obj/electronic_circuit.o
EXTRA_FLAGS=-I include -I ../maths/include -I scan/ -std=c++17

include ../../basic_tests.mk
include ../../basic_compilation_fwd.mk

$(call create-io-tests,incidence-matrix-test,bin/test/calc_currents -m,test/data/incidence_matrix)
$(call create-io-tests,voltage-srcs-matrix-test,bin/test/calc_currents -v,test/data/voltage_srcs_matrix)
$(call create-io-tests,current-srcs-matrix-test,bin/test/calc_currents -i,test/data/current_srcs_matrix)
$(call create-io-tests,conductance-matrix-test,bin/test/calc_currents -g,test/data/conductance_matrix)
$(call create-io-tests,calc-currents-test,bin/test/calc_currents,test/data/currents)

DIFFPROG = $(1) | awk '{ printf("%d -- %d: %.f A\n", $$$$1, $$$$3, $$$$4) }' | diff - $(2) -bB --brief
$(call create-io-tests,calc-currents-round-test,bin/test/calc_currents,test/data/currents/round)

all: scan/scan.h compile-essential-objs bin/test/calc_currents $(incidence-matrix-test)\
	$(voltage-srcs-matrix-test) $(current-srcs-matrix-test) $(conductance-matrix-test) $(calc-currents-test)\
	$(calc-currents-round-test)

scan/scan.h:
	git submodule init && git submodule update

$(call add-extra-linkage-flags,bin/test/calc_currents,-lboost_program_options-mt)

include ../../basic_compilation_bwd.mk
