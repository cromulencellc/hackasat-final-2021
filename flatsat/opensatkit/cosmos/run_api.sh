#!/bin/bash

PUID=$(id -u)
PGID=$(id -g)
CWD=$(pwd)

echo "Start Cosmos"
docker run \
    -it \
    --rm \
    -e DISPLAY \
    -e QT_X11_NO_MITSHM=1 \
    -e QT_GRAPHICSSYSTEM='native' \
    --user ${PUID}:${GUID} \
    -v ${CWD}:/cosmos \
    --name cosmos_api \
    has2/finals/opensatkit-docker/cosmos:latest