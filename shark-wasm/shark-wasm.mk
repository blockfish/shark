SHARK_WASM = ${ROOT}/shark-wasm
SHARK_WASM_BUILDDIR = ${BUILD}/shark-wasm

SHARK_WASM_SETUP_ARGS = -Dcpp_std=c++20
SHARK_WASM_SETUP_ARGS += -Doptimization=s
SHARK_WASM_SETUP_ARGS += --cross-file ${EMSCRIPTEN_CROSS_FILE}

SHARK_WASM_SETUP_ARGS += -D"shark:ruleset_json"=${SRS_JSON}

www: \
	${WWW}/webworker.js \
	${WWW}/webworker.wasm

${WWW}/webworker.js: ${SHARK_WASM_BUILDDIR}/webworker.js
	@mkdir -p $(dir $@)
	cp -u $< $@

${WWW}/webworker.wasm: ${SHARK_WASM_BUILDDIR}/webworker.js
	@mkdir -p $(dir $@)
	cp -u ${SHARK_WASM_BUILDDIR}/webworker.wasm $@

.PHONY: ${SHARK_WASM_BUILDDIR}/webworker.js
${SHARK_WASM_BUILDDIR}/webworker.js: ${SHARK_WASM_BUILDDIR}/build.ninja
	${NINJA} -C $(dir $@) -v

${SHARK_WASM_BUILDDIR}/build.ninja: ${EMSCRIPTEN_CROSS_FILE}
	${MESON} setup $(dir $@) ${SHARK_WASM} \
		${SHARK_WASM_SETUP_ARGS}
	@touch $@

.PHONY: clean-shark-wasm
clean-shark-wasm:
	rm -rf ${SHARK_WASM_BUILDDIR} ${WWW}/webworker.js ${WWW}/webworker.wasm
