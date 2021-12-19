EMSDK = ${ROOT}/support/third_party/emsdk
EMSDK_VERSION ?= 3.0.0

setup: setup-emscripten
.PHONY: setup-emscripten

setup-emscripten:
	${EMSDK}/emsdk install ${EMSDK_VERSION}
	${EMSDK}/emsdk activate ${EMSDK_VERSION}

EMSCRIPTEN_CROSS_FILE = ${BUILD}/emscripten.ini
EMSCRIPTEN_PATH = $(abspath ${EMSDK}/upstream/emscripten)

${EMSCRIPTEN_CROSS_FILE}: ${ROOT}/support/emscripten.ini
	@mkdir -p $(dir $@)
	echo "[constants]" > $@
	echo "toolchain = '${EMSCRIPTEN_PATH}'" >> $@
	cat $< >> $@
