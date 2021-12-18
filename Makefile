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
${BUILD}/.setup: # automatically run setup on first build
	make setup
	@mkdir -p $(dir $@) && touch $@
all: ${BUILD}/.setup
.PHONY: setup

# meta target for the website
www:
all: www
.PHONY: www

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

include support/node.mk
include support/http-server.mk

include tank/tank.mk
