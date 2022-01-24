

TAG ?= has2/finals/fax/digest:latest
SHORT_TAG ?= has2/finals/fax/digest:latest

.PHONY: default
default: build push

.PHONY: build
build:
	docker build -t $(TAG) . && \
	docker tag $(TAG) $(SHORT_TAG)

.PHONY: save
save:
	docker save -o digest.tar $(TAG)

.PHONY: push
push:
	docker push $(TAG)