#---------------------------------------------------------
#  This file is intended to be included in other makefiles to run
# unit-tests and input-output test simplier
#---------------------------------------------------------
# Usage:
# 1) create-io-tests function
#  Should be called outside targets similar to this:
# $(call create-io-tests,name-of-group,program-to-run,directory-with-data)
#  After that, $(name-of-group) can be used as a prerequisite in some target
# to run this test with all suitable files in directory-with-data
#  Input files must have suffix .test-in, files for output comparison - .test-out.
# Other files in directory-with-data are ignored
#  Input files are passed to stdin of program-to-run, output files are compared
# with stdout of that programm
#  Note. create-io-tests cannot be instantiated with the same name-of-group and
# directory-with-data more than once
#
# 2) add-unit-test-to-group function
#  Should be called outside targets similar to this:
# $(call add-unit-test-to-group,unit-tests-group,binary-file-with-test-1)
# $(call add-unit-test-to-group,unit-tests-group,binary-file-with-test-2)
# # ... etc
#  After that, if $(unit-tests-group) is used as a prerequisite in some target,
# binary-file-with-test-1, binary-file-with-test-2 and will be called
#  Each group may contain any number of tests
#  After call, added unit-test binary will have semantics of secondary files,
# i.e. it will not be deleted automatically, even if it will be made by
# implicit rule
#---------------------------------------------------------
# author: https://github.com/graudtV
#---------------------------------------------------------


#---- input-output tests impl. ----#

# $(1) - progname, $(2) - test name with .test suffix
define run-diff-test
$(1) < $(patsubst %.test,%.test-in,$(2)) | diff - $(patsubst %.test,%.test-out,$(2)) -bB --brief
endef

# determinant-tests: $(determinant-tests-targets)
define _io-individual-test-template
$(1): $(word 1,$(2))
	$(call run-diff-test,$(2),$(1))
endef

# $(1) - name of group, $(2) - program to run, $(3) - directory with data
define create-io-tests
$(eval _targets = $(addsuffix .test, $(basename $(wildcard $(3)/*.test-in))))
PHONY: $(_targets)
$(eval $(1)=$(_targets))
$(foreach test,$(value _targets),$(eval $(call _io-individual-test-template,$(test),$(2))))
endef

#---- unit-tests impl. ----#

define _individual-unit-test-template
$(1)+=$(2)._unit-test
$(2)._unit-test: $(2)
	$(2)
endef

add-unit-test-to-group=$(eval $(call _individual-unit-test-template,$(1),$(2)))

#---- other ---- #
%._run: %
	$<
