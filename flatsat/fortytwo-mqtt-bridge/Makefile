#
# Make wrapper for fortytwo-bridge
#


TAG ?= has2/fortytwobrige:latest

.PHONY: build
build:
	docker build -t $(TAG) .

.PHONY: run
run:
	docker run -it --rm $(TAG)
