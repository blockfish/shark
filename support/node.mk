NPM ?= npm
NPX ?= npx

.PHONY: setup-node clean-node
setup: setup-node
clean: clean-node

setup-node:
	${NPM} install

clean-node:
	rm -rf node_modules
