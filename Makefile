################################################################################
# global paths
################################################################################

# project root
ROOT = $(abspath .)
# build artifacts
BUILD ?= ${ROOT}/build
# outputs
DIST ?= ${ROOT}/dist
# website outputs
WWW = ${DIST}/www

################################################################################
# meta-targets
################################################################################

# meta target to build everything
all:
.PHONY: all

# meta target to set up dependencies
setup:
${ROOT}/.did_setup: # automatically run setup on first build
	make setup
	@mkdir -p $(dir $@) && touch $@
all: ${ROOT}/.did_setup
.PHONY: setup

# meta target for the website
www:
all: www
.PHONY: www

# meta target for command line tools
cli:
all: cli
.PHONY: cli

# meta target to clean everything built
clean: clean-build clean-dist
clean-build:
	rm -rf ${BUILD}
clean-dist:
	rm -rf ${DIST}
.PHONY: clean clean-build clean-dist

################################################################################
# includes
################################################################################

include support/emscripten.mk
include support/node.mk
include support/http-server.mk

include shark-cli/shark-cli.mk
include shark-wasm/shark-wasm.mk
include tank/tank.mk
