MESON ?= meson
NINJA ?= ninja

SRS_JSON ?= ${ROOT}/support/srs.json

SHARK = ${ROOT}/shark
SHARK_BUILDDIR = ${BUILD}/shark

SHARK_SETUP_ARGS = -Dcpp_std=c++20
SHARK_SETUP_ARGS += -Druleset_json=${SRS_JSON}
SHARK_SETUP_ARGS += -Dcompile_tests=true
SHARK_SETUP_ARGS += -Doptimization=0

test: \
	run-shark-tests

run-shark-tests: ${SHARK_BUILDDIR}/run-tests
	$<

.PHONY: ${SHARK_BUILDDIR}/run-tests
${SHARK_BUILDDIR}/run-tests: ${SHARK_BUILDDIR}/ninja.build
	${NINJA} -C $(dir $<)

${SHARK_BUILDDIR}/ninja.build:
	${MESON} setup $(dir $@) ${SHARK} \
		${SHARK_SETUP_ARGS}
	@touch $@

.PHONY: clean-shark-tests
clean-shark-tests:
	rm -rf ${SHARK_BUILDDIR}

