MESON ?= meson
NINJA ?= ninja

SHARK = ${ROOT}/shark

SHARK_CLI_BUILDDIR = ${BUILD}/shark-cli

SHARK_RULESET_JSON = ${ROOT}/support/srs.json

SHARK_CLI_SETUP_ARGS = --buildtype release
SHARK_CLI_SETUP_ARGS += -Doptimization=s
#SHARK_CLI_SETUP_ARGS += -Dwarning_level=3
SHARK_CLI_SETUP_ARGS += -Dblockfish_ruleset_json=${SHARK_RULESET_JSON}

cli: \
	${DIST}/shark

${DIST}/shark: ${SHARK_CLI_BUILDDIR}/cli
	@mkdir -p $(dir $@)
	cp $< $@

${SHARK_CLI_BUILDDIR}/cli: ${SHARK_CLI_BUILDDIR}/ninja.build \
		$(shell find ${SHARK}/src) ${SHARK_RULESET_JSON}
	${NINJA} -C $(dir $<) \
		&& touch $@

${SHARK_CLI_BUILDDIR}/ninja.build:
	${MESON} setup $(dir $@) ${SHARK} \
		${SHARK_CLI_SETUP_ARGS} \
		&& touch $@

.PHONY: regen-shark-cli
regen-shark-cli:
	rm -rf ${SHARK_CLI_BUILDDIR}
	make ${SHARK_CLI_BUILDDIR}/ninja.build

