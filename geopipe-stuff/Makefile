.PHONY: all clean configure deps dist distclean lint

BUILD_ENV_CHECKSUM := $(shell (env | sort; cat wscript; cat Makefile) | cksum)
LAST_BUILD_ENV_CHECKSUM := $(shell \
	if [ -f build/last_build ]; then \
		cat build/last_build; \
	fi)

# Ensure that we have build/targets.mk
IGNORE_OUTPUT := $(shell \
	if [ ! -f build/targets.mk ]; then \
		./waf configure mkgen; \
	fi > /dev/null 2>&1)

WAF_PRELUDE ?=

ifneq ($(BUILD_ENV_CHECKSUM), $(LAST_BUILD_ENV_CHECKSUM))
	# Reconfigure if necessary, and regenerate build/targets.mk
	WAF_PRELUDE += @make configure;
	WAF_PRELUDE += echo $(BUILD_ENV_CHECKSUM) > build/last_build;
endif

WAF ?= @scripts/run_activated.sh ./waf
V ?= 0
J ?= 0

ifneq (0, $(V))
	WAF += -v
endif

ifneq (0, $(J))
	WAF += -j$(J)
endif

all:
	$(WAF_PRELUDE)
	$(WAF)

clean:
	$(WAF_PRELUDE)
	$(WAF) clean

configure:
	$(WAF) configure mkgen

deps:
	$(WAF_PRELUDE)
	$(WAF) builddeps

dist:
	$(WAF) dist

distclean:
	$(WAF) distclean

lint:
	$(WAF) lint

-include build/targets.mk
