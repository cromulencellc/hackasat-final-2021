#!/usr/bin/bash

usage="Usage: just run me to kill everything"

all_defined=0

while getopts "h:a" option; 
do
    case "$option" in
        h|\?)
            echo ${usage};
            exit;;
        a )
            all_defined=1
            ;;
        : )
            echo "Missing option argument for -$OPTARG" >&2; exit 1;;
        *  )
            echo "Unimplemented option: -$OPTARG" >&2; exit 1;;
    esac
done

# Kills the terminator window, triggering flatsat.sh to die
echo "Stop terminator"
pkill -e terminator

source .env

echo "Stop Docker Services"
docker-compose down 

if [ ${all_defined} -eq 1 ]; then
    echo "Delete Docker Network ${DOCKER_NETWORK}"
    docker network rm ${DOCKER_NETWORK}
    echo "Delete shim ${DOCKER_NETWORK}-shim"
    sudo ip link del "${DOCKER_NETWORK}-shim"
fi

if [ -f .team${TEAM_NUMBER} ]; then
    rm .team${TEAM_NUMBER}
fi

