#
# Make wrapper for payload docker build
#

TAG ?= has2/finals/comms/payload-dispatcher-arm64:latest
SHORT_TAG ?= has2/finals/comms/payload-dispatcher-arm64:latest

.PHONY: default
default: deps build save

.PHONY: deps
deps: 
	sudo apt-get update && sudo apt-get install -y binfmt-support qemu-user-static


.PHONY: build
build:
	docker buildx build --platform linux/arm64 -t $(TAG) . && \
	docker tag $(TAG) $(SHORT_TAG)

.PHONY: save
save:
	docker save -o payload-dispatcher-arm64.tar $(TAG)
	
.PHONY: push
push:
	docker push $(TAG)

# .PHONY: build-asyncio
# build-asyncio:
# 	docker buildx build --platform linux/arm64 -t $(TAG) -f Dockerfile-async . && \
# 	docker tag $(TAG) $(SHORT_TAG)
	

