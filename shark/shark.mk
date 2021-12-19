MESON ?= meson
NINJA ?= ninja

SHARK = ${ROOT}/shark

SHARK_CLI_BUILDDIR = ${BUILD}/shark-cli

SHARK_CLI_SETUP_ARGS = --buildtype release
SHARK_CLI_SETUP_ARGS += -Doptimization=s
SHARK_CLI_SETUP_ARGS += -Dwarning_level=3

cli: \
	${DIST}/shark

${DIST}/shark: ${SHARK_CLI_BUILDDIR}/cli
	@mkdir -p $(dir $@)
	cp $< $@

${SHARK_CLI_BUILDDIR}/cli: ${SHARK_CLI_BUILDDIR}/ninja.build $(shell find ${SHARK}/src)
	${NINJA} -C $(dir $<) && touch $@

${SHARK_CLI_BUILDDIR}/ninja.build:
	${MESON} setup $(dir $@) ${SHARK} && touch $@
