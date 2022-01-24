#
# Make wrapper for adcs-drivers
#

O ?= build
P ?= adcs_drivers_runtime

.PHONY: default 
default: all

.PHONY: all
all: clean prep configure build package

.PHONY: cross
cross: clean prep configure_cross build_cross package

clean:
	rm -rf "$(O)"
	rm -rf "$(P)"

prep:
	mkdir -p "${O}"

.PHONY: configure
configure: prep
	cd "$(O)" && \
	cmake -DCMAKE_BUILD_TYPE=Debug -DLOG_LEVEL=4 ..

.PHONY: configure_cross
configure_cross:
	cd "$(O)" && \
	cmake -DCMAKE_BUILD_TYPE=Debug -DLOG_LEVEL=3 -DCMAKE_TOOLCHAIN_FILE=arm-linux-gnueabihf_toolchain.cmake ..

.PHONY: build_cross
build_cross:
	cd "$(O)" && \
	cmake --build .

.PHONY: build
build:
	cd "$(O)" && \
	cmake --build . -- -j 4
	# cmake --build .
	

.PHONY: test
test:
	(cd $(O) && ctest -O ctest.log)

.PHONY: package
package:
	mkdir -p "${P}/bin/"
	cp build/cubewheel-driver/test/test-cubewheel ${P}/bin/
	cp build/bno08x-driver/test/bno08x_cal ${P}/bin/
	cp build/bno08x-driver/test/test-bno08x-driver ${P}/bin/
	cp build/gpio-wrapper/test/test-gpio-driver ${P}/bin/
	cp build/torquerod-driver/test/test-torquerod-driver ${P}/bin/
	cp build/xadc-driver/test/test-xadc-driver ${P}/bin/
	tar -cvzf adcs-drivers-test-package.tar.gz ${P}/

.PHONY: docker-build
docker-build:
		docker run \
		-it \
		--rm \
		-u $(shell id -u):$(shell id -g) \
		-v $(shell pwd):/apps \
		--name adcs-drivers-build \
		--entrypoint="/bin/bash" \
		has2/finals/opensatkit-docker/cfs-builder/armhf:latest \
		-c 'make cross'
		





