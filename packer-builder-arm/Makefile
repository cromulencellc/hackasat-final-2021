#
# Make wrapper for packer-builder-arm
#

TAG ?= has2/finals/packer-builder-arm/packer-builder-arm:latest

.PHONY: default 
default: build-comm-image publish-comm-image

.PHONY: docker-build
docker-build:
	docker build -t $(TAG) -f docker/Dockerfile .
	docker tag $(TAG) packer-builder-arm:latest

.PHONY: build-comm-image
build-comm-image:
	docker run --rm --privileged -v /dev:/dev -v $(shell pwd):/build packer-builder-arm:latest build boards/hack-a-sat/comm_ubuntu_server_20.04_arm64.json

.PHONY: build-comm-image-gnuradio9
build-comm-image-gnuradio9:
	docker run --rm --privileged -v /dev:/dev -v $(shell pwd):/build packer-builder-arm:latest build boards/hack-a-sat/comm_ubuntu_server_20.04_arm64_gnuradio9.json

.PHONY: build-adcs-image
build-adcs-image:
	docker run --rm --privileged -v /dev:/dev -v $(shell pwd):/build packer-builder-arm:latest build boards/hack-a-sat/adcs_ubuntu_base_20.04_armhf.json

.PHONY: build-adcs-image-18
build-adcs-image-18:
	docker run --rm --privileged -v /dev:/dev -v $(shell pwd):/build packer-builder-arm:latest build boards/hack-a-sat/adcs_ubuntu_base_18.04_armhf.json
