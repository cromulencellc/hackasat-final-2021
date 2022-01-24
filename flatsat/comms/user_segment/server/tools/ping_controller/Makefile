REMOTE_DOCKER_TAG=has2/finals/comms/user-seg-ping-controller:latest
LOCAL_DOCKER_TAG=ping_controller:server

docker_build:
	docker build -t $(LOCAL_DOCKER_TAG) .
docker_pull:
	docker pull has2/finals/comms/user-seg-ping-controller:latest
run_local:
	docker run -it --rm --network host --user $(UID):$(GID) -v "${CURDIR}/../user_segment/configs/:/configs" $(LOCAL_DOCKER_TAG) --config /configs
run: 
	docker run -it --rm --network host --user $(UID):$(GID) -v "${CURDIR}/../user_segment/configs/:/configs" $(REMOTE_DOCKER_TAG)
run_debug: 
	docker run -it --rm --network host --user $(UID):$(GID) -v "${CURDIR}/../user_segment//configs/:/configs" $(REMOTE_DOCKER_TAG)
