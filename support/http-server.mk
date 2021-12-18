# THIS HTTP SERVER IS FOR DEVELOPMENT USE ONLY!

HTTP_SERVER_PORT ?= 8080

start-http-server: www
	${NPM} install --no-save http-server
	${NPX} http-server \
		${WWW} \
		-p ${HTTP_SERVER_PORT} -c-1
