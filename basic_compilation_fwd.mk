
BASIC_COMPILATION_FWD_INCLUDED_=true

# $(1) - target binart, $(2) - flags
define set-explicit-linkage-flags
$(eval $(1)._linkage_flags=$(2))
endef

define add-extra-linkage-flags
$(if $($(1)._linkage_flags),$(eval $(1)._linkage_flags+=$(2)),$(eval $(1)._linkage_flags=$(EXTRA_FLAGS) $(CXXFLAGS) $(2)))
endef