TANK = ${ROOT}/tank

TANK_SASS = sass
TANK_SASS_ARGS = --no-error-css

TANK_ESBUILD = ${NPX} esbuild
TANK_ESBUILD_ARGS  = --bundle --platform=browser
TANK_ESBUILD_ARGS += --sourcemap=inline

TANK_SASS_ARGS += --no-color
TANK_ESBUILD_ARGS += --color=false

www: \
	${WWW}/index.html \
	${WWW}/style.css \
	${WWW}/bundle.js

${WWW}/index.html: ${TANK}/static/index.html
	@mkdir -p $(dir $@)
	cp $< $@

${WWW}/bundle.js: $(shell find ${TANK}/app -name '*.js*')
	@mkdir -p $(dir $@)
	${TANK_ESBUILD} ${TANK_ESBUILD_ARGS} \
		${TANK}/app \
		--outfile=$@

${WWW}/style.css: $(shell find ${TANK}/style -name '*.scss')
	@mkdir -p $(dir $@)
	${TANK_SASS} ${TANK_SASS_ARGS} \
		${TANK}/style/index.scss \
		$@

# TODO: minify
