#
# Make wrapper for 42
#

O ?= build
TAG ?= has2/42


.PHONY: default 
default: all

.PHONY: all
all: clean prep configure build

# .PHONY: cross
# cross: clean prep configure_cross build_cross package

clean:
	rm -rf "$(O)" && \
   if [ -f ./42 ]; then rm ./42; fi; \
   if [ -f ./FlatSatFswApp ]; then rm ./FlatSatFswApp; fi;
	# rm -rf "$(P)"
   

prep:
	mkdir -p "${O}"

start_venv:
	. ./.venv-42/bin/activate

.PHONY: configure
configure: prep
	cd "$(O)" && \
	cmake -DCMAKE_BUILD_TYPE=Debug  ..

.PHONY: install_deps
install_deps:
	sudo apt install -y build-essential python3 python3-venv && \
	python3 -m venv .venv-42 && \
	. ./.venv-42/bin/activate && \
	pip3 install -r requirements.txt


.PHONY: build
build:
	cd "$(O)" && \
	cmake --build . -- -j $(shell nproc) && \
	cmake --build . --target install

.PHONY: test
test: start_venv
	(cd $(O) && ctest -O ctest.log)

.PHONY: docker-build
docker-build:
	docker build -t $(TAG) .

.PHONY: docker-run
docker-run:
	docker run -it --rm -v $(PWD)/HAS2:/home/has/42/HAS2 -e TEAM_NUMBER=1 -e MQTT_IP=localhost $(TAG)





