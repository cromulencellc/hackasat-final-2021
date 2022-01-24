UID ?= $(shell id -u)
GID ?= $(shell id -g)

.PHONY: cosmos
cosmos: cosmos_4.5.0
	

.PHONY: cosmos_4.5.0
cosmos_4.5.0: Dockerfile
	@echo Using $(UID):$(GID)
	docker image build \
		-t has2/finals/opensatkit-docker/cosmos:latest \
		--build-arg UID=$(UID) \
		--build-arg GID=$(GID) \
		. && \