TAG ?= has2/finals/comms/comms-challenge-telemetry-arm64:latest
SHORT_TAG ?= has2/finals/comms/comms-challenge-telemetry-arm64:latest
FILE ?= comms-challenge-telemetry-arm64.tar

.PHONY: default
default: build save push

.PHONY: build
build:
	docker buildx build --platform linux/arm64 -t $(TAG) . && \
	docker tag $(TAG) $(SHORT_TAG)

.PHONY: save
save:
	docker save -o $(FILE) $(TAG)
	
.PHONY: push
push:
	docker push $(TAG)