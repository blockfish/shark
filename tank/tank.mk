TANK = ${ROOT}/tank

www: \
	${WWW}/index.html

${WWW}/index.html: ${TANK}/static/index.html
	@mkdir -p $(dir $@)
	cp $< $@
