

TAG ?= has2/finals/fax/raw:latest
SHORT_TAG ?= has2/finals/fax/raw:latest

.PHONY: default
default: build save push

.PHONY: build
build:
	docker build -t $(TAG) . && \
	docker tag $(TAG) $(SHORT_TAG)

.PHONY: save
save:
	docker save -o raw.tar $(TAG)

.PHONY: push
push:
	docker push $(TAG)