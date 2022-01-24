#
# Make wrapper for fax docker build
#

RAW_PORT ?= 1337
DIGEST_PORT ?= 134
FLAG ?= TESTFLAG1234

.PHONY: default
default: build-raw build-digest

.PHONY: build-raw
build-raw:
	$(MAKE) -C raw/

.PHONY: build-digest
build-digest:
	$(MAKE) -C digest/

.PHONY: test
test: build-raw build-digest
	docker run --rm -e SERVER_PORT=${RAW_PORT} has2/finals/fax/raw:latest & 
	docker run --rm -e SERVER_PORT=${DIGEST_PORT}1 -e FLAG=${FLAG} has2/finals/fax/digest:latest &
	docker run --rm -e SERVER_PORT=${DIGEST_PORT}2 -e FLAG=${FLAG} has2/finals/fax/digest:latest &
	docker run --rm -e SERVER_PORT=${DIGEST_PORT}3 -e FLAG=${FLAG} has2/finals/fax/digest:latest &
	docker run --rm -e SERVER_PORT=${DIGEST_PORT}4 -e FLAG=${FLAG} has2/finals/fax/digest:latest &
	docker run --rm -e SERVER_PORT=${DIGEST_PORT}5 -e FLAG=${FLAG} has2/finals/fax/digest:latest &
	docker run --rm -e SERVER_PORT=${DIGEST_PORT}6 -e FLAG=${FLAG} has2/finals/fax/digest:latest &
	docker run --rm -e SERVER_PORT=${DIGEST_PORT}7 -e FLAG=${FLAG} has2/finals/fax/digest:latest &
	docker run --rm -e SERVER_PORT=${DIGEST_PORT}8 -e FLAG=${FLAG} has2/finals/fax/digest:latest &


	

