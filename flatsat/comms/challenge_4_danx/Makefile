#
# Make wrapper for payload docker build
#

TAG ?= has2/finals/comms/comms-challenge-arm64:latest
SHORT_TAG ?= has2/finals/comms/comms-challenge-arm64:latest
FILE ?= comms-challenge-arm64.tar

.PHONY: default
default: compile build telemetry save push

.PHONY: telemetry
telemetry:
	$(MAKE) -C telemetry/

.PHONY: deps
deps: 
	sudo apt-get update && sudo apt-get install -y binfmt-support qemu-user-static gcc-aarch64-linux-gnu

compile:
	mkdir -p build
	cp libs/aarch64/libc-2.28.so libs/aarch64/ld-2.28.so libs/aarch64/comm build

compile-x86:
	mkdir -p build
	cp libs/x86_64/libc-2.28.so libs/x86_64/ld-2.28.so build
	gcc -g -no-pie -o build/comm comm.c

.PHONY: build
build:
	docker buildx build --platform linux/arm64 -t $(TAG) . && \
	docker tag $(TAG) $(SHORT_TAG)

.PHONY: build-x86
build-x86:
	docker build -t $(TAG) . && \
	docker tag $(TAG) $(SHORT_TAG)

.PHONY: save
save:
	docker save -o $(FILE) $(TAG)
	
.PHONY: push
push:
	docker push $(TAG)

# .PHONY: build-asyncio
# build-asyncio:
# 	docker buildx build --platform linux/arm64 -t $(TAG) -f Dockerfile-async . && \
# 	docker tag $(TAG) $(SHORT_TAG)
	

