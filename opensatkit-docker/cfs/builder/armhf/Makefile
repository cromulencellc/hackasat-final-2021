UID ?= $(shell id -u)
GID ?= $(shell id -g)
GITLAB_TOKEN ?= $(CI_JOB_TOKEN)
ROOTFS_VERSION ?= 1.0.0
ROOTFS_VERSION_18 ?= 1.0.18

.PHONY: armhf
armhf: 
	@echo Using $(UID):$(GID)
	docker image build \
		-t has2/finals/opensatkit-docker/cfs-builder/armhf:latest \
		--build-arg UID=$(UID) \
		--build-arg GID=$(GID) \
		. && \
