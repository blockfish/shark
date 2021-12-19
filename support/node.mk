NPM ?= npm
NPX ?= npx

.PHONY: setup-node clean-node
setup: setup-node

setup-node:
	${NPM} install
