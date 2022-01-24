#!/bin/bash

SERVERIP="10.0.0.101"
CONTROLIP="10.0.0.102"
CONTROL_REMOTE_TAG="has2/finals/comms/user-seg-ping-controller:latest"
CONTROL_LOCAL_TAG="ping_controller:server"
SERVER_REMOTE_TAG="has2/finals/comms/user-seg-server:latest"
SERVER_LOCAL_TAG="user_segment:server"
CONTROL_TAG=${CONTROL_REMOTE_TAG}
SERVER_TAG=${SERVER_REMOTE_TAG}

usage='Usage: start.sh -a "<WRAP IN QUOTES| additional_args pass into server.py>" \n\t\t-s (start server) \n\t\t-c (start controller) \n\t\t-d (use local docker container for developement)'

ipaddr=$(ip route get 8.8.8.8 | awk -F"src " 'NR==1{split($2,a," ");print a[1]}')

start_server=0
start_controller=0
dev_local=0
extra_args=""
while getopts "h:a:cds" option;
do
    case "$option" in
        h|\?)
            echo -e ${usage}
            exit;;
        a )
            extra_args=$OPTARG;;
        c )
            start_controller=1;;
        d )
            dev_local=1;;
        s )
            start_server=1;;
        : )
            echo "Missing option argument for -$OPTARG" >&2; exit 1;;
        *  )
            echo "Unimplemented option: -$OPTARG" >&2; exit 1;;
    esac
done

# Setup .env file that docker-compose automatically loads
GUID=$(id -g)
echo -e "UID=${UID}" > .env
echo -e "GUID=${GUID}" >> .env
echo -e "EXTRA_ARGS=\"$extra_args\"" >> .env
if [ $dev_local -eq 1 ]; then
    CONTROL_TAG="${CONTROL_LOCAL_TAG}"
    SERVER_TAG="${SERVER_LOCAL_TAG}"
fi
echo -e "CONTROL_TAG=${CONTROL_TAG}" >> .env
echo -e "SERVER_TAG=${SERVER_TAG}" >> .env

# Start up either the server or the controller
if [ $start_server -eq 1 ]; then
    docker-compose up server
elif [ $start_controller -eq 1 ]; then
    docker-compose up controller
elif [ $ipaddr == $SERVERIP ]; then
    echo "Detected this is the server. Starting it up."
    docker-compose up server
elif [ $ipaddr == $CONTROLIP ]; then
    echo "Detected this is the radio box. Starting up controller."
    docker-compose up controller
else
    echo -e ${usage}
fi
