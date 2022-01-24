#
# Make wrapper for 42
#

O ?= build
TAG ?= has2/pdb-builder
TEAM_NUMBER ?= 1
PIO_ENV := team_$(TEAM_NUMBER)
CWD = $(shell pwd)



.PHONY: default 
default: all

.PHONY: all
all: clean upload upload

# .PHONY: cross
# cross: clean prep configure_cross build_cross package
.PHONY: clean
clean:
	docker run -it --rm -v $(PWD):/mnt $(TAG) run -e $(PIO_ENV) --target clean

.PHONY: upload
upload:
	docker run -it --rm -v $(PWD):/mnt --privileged -v /dev:/dev/ $(TAG) run  -e $(PIO_ENV) --target upload

.PHONY: build
build:
	docker run -it --rm -v $(PWD):/mnt $(TAG) run  -e $(PIO_ENV)

.PHONY: docker-build
docker-build:
	docker build -t $(TAG) .

