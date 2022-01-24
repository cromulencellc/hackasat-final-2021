TAG ?= has2/finals/comms/user-seg-server:latest
LOCAL_TAG ?= user_segment:server

docker_build:
	docker build -t $(LOCAL_TAG) .
docker_pull:
	docker pull $(TAG)
run: server.py
	docker run -it --rm --network host --user $(UID):$(GID) -v "$(shell pwd)/configs/:/configs" $(TAG) --dispatch -r 0.0.0.0 --config /configs
run_debug: server.py
	docker run -it --rm --network host --user $(UID):$(GID) -v "$(shell pwd)/configs/:/configs" $(TAG) --dispatch --config /configs -vvvv
run_local_debug: server.py
	docker run -it --rm --network host --user $(UID):$(GID) -v "$(shell pwd)/configs/:/configs" $(LOCAL_TAG) -r 0.0.0.0 --dispatch --config /configs -vvvv
keys_only:
	docker run -it --rm --user $(UID):$(GID) -v "$(shell pwd)/configs/:/configs" $(TAG) --generate --config /configs -vv