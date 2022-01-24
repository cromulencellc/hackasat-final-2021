#!/usr/bin/bash

cd /home/comm/payload

export MQTT_IP=@MQTT_IP_ADDR@
export FLAG=@PAYLOAD_DISPATCHER_FLAG@
export TEAM_NUMBER=${TEAM_NUMBER}

mkdir -p $(pwd)/dispatcher/logs

docker run -it --rm \
    --pids-limit 100 \
    -e FLAG=${FLAG} \
    -e MQTT_USER=comm \
    -e MQTT_PASS=2B5458962584FE417B2803CF \
    -e MQTT_HOST=${MQTT_IP} \
    -e TEAM_NUMBER=${TEAM_NUMBER} \
    --name payload_dispatcher \
    -v $(pwd)/dispatcher/logs:/payload/logs \
    -p 127.0.0.1:2303:2303/udp \
    --add-host=host.docker.internal:host-gateway \
    has2/finals/comms/payload-dispatcher-arm64 \
    "-m NORMAL"
