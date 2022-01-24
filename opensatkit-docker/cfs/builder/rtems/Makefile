UID ?= $(shell id -u)
GID ?= $(shell id -g)
TAG ?= has2/finals/opensatkit-docker/cfs-builder/rtems:latest

.PHONY: rtems
rtems: rtems_5.0.0
	

.PHONY: rtems_5.0.0
rtems_5.0.0: Dockerfile
	@echo Using $(UID):$(GID)
	docker image build \
		-t $(TAG) \
		--build-arg UID=$(UID) \
		--build-arg GID=$(GID) \
		. && \

.PHONY: get_rtems
get_rtems:
	rm -rf rtems && mkdir rtems
	echo "Start container"
	docker run --entrypoint="/bin/bash" --name "get_rtems" opensatkit-docker/cfs-builder/rtems:latest "-f" "/dev/null"
	echo "Copy out rtems"
	docker cp -L get_rtems:/opt/rtems rtems
	docker rm get_rtems
