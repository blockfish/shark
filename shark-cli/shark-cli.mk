MESON ?= meson
NINJA ?= ninja

SRS_JSON ?= ${ROOT}/support/srs.json

SHARK_CLI = ${ROOT}/shark-cli
SHARK_CLI_BUILDDIR = ${BUILD}/shark-cli

SHARK_CLI_SETUP_ARGS = -Dcpp_std=c++20

SHARK_CLI_SETUP_ARGS += -D"shark:ruleset_json"=${SRS_JSON}

SHARK_CLI_SETUP_ARGS += --buildtype release
SHARK_CLI_SETUP_ARGS += -Doptimization=3
SHARK_CLI_SETUP_ARGS += -Db_lto=true

cli: \
	${DIST}/shark

${DIST}/shark: ${SHARK_CLI_BUILDDIR}/cli
	@mkdir -p $(dir $@)
	cp -u $< $@

.PHONY: ${SHARK_CLI_BUILDDIR}/cli
${SHARK_CLI_BUILDDIR}/cli: ${SHARK_CLI_BUILDDIR}/ninja.build
	${NINJA} -C $(dir $<)

${SHARK_CLI_BUILDDIR}/ninja.build:
	${MESON} setup $(dir $@) ${SHARK_CLI} \
		${SHARK_CLI_SETUP_ARGS}
	@touch $@

.PHONY: clean-shark-cli
clean-shark-cli:
	rm -rf ${SHARK_CLI_BUILDDIR} ${DIST}/shark
