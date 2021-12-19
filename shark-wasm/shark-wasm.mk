MESON ?= meson
NINJA ?= ninja

SHARK_WASM = ${ROOT}/shark-wasm
SHARK_WASM_BUILDDIR = ${BUILD}/shark-wasm
SHARK_WASM_BUILDFILE = ${SHARK_WASM_BUILDDIR}/ninja.build

SHARK_WASM_CROSSFILE = ${SHARK_WASM_BUILDDIR}/emscripten.txt

SHARK_WASM_SETUP_ARGS = -Dcpp_std=c++20
SHARK_WASM_SETUP_ARGS += -Doptimization=s
SHARK_WASM_SETUP_ARGS += --cross-file ${EMSCRIPTEN_CROSS_FILE}

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
${SHARK_WASM_BUILDDIR}/webworker.js: ${SHARK_WASM_BUILDFILE}
	${NINJA} -C $(dir $@) -v

force-shark-wasm-rebuild:

${SHARK_WASM_BUILDFILE}: ${EMSCRIPTEN_CROSS_FILE}
	${MESON} setup $(dir $@) ${SHARK_WASM} \
		${SHARK_WASM_SETUP_ARGS}
	@touch $@


