#
# Make wrapper for bno08x-driver
#

O ?= build

.PHONY: default
default: all

.PHONY: all
all: clean build

clean:
	rm -rf "$(O)"

build:
	mkdir "${O}" && \
	cd "${O}" && \
	cmake -DCMAKE_BUILD_TYPE=Debug .. && \
	cmake --build .

test:
	(cd $(O) && ctest -O ctest.log)

