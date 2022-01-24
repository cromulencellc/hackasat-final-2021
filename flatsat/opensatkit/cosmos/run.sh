#!/bin/bash

PUID=$(id -u)
PGID=$(id -g)
CWD=$(pwd)

function usage {
    echo "Usage: run.sh [-t <target_file(optional)>  (system_has.txt) -h (help)] Runs system.txt with no options."
}

system_flag=0
system_file=""
team_num=""
team_defined=0

while getopts "h?s:t:" option; 
do
    case "$option" in
        h|\?)
            usage;
            exit;;
        s ) 
            system_file=$OPTARG
            system_flag=1
            ;;
        t )
            team_num=$OPTARG
            team_defined=1
            ;;
        : )
            echo "Missing option argument for -$OPTARG" >&2; exit 1;;
        *  )
            echo "Unimplemented option: -$OPTARG" >&2; exit 1;;
    esac
done

SUBNET_BASE="192.168"
TEAM_SUBNET_NUM=$(expr ${team_num} + 100)
FLATSAT_SUBNET="${SUBNET_BASE}.${TEAM_SUBNET_NUM}"
HOST_IP="${FLATSAT_SUBNET}.2"
# COSMOS_IP="${FLATSAT_SUBNET}.3"
CDH_IP="${FLATSAT_SUBNET}.67"
PDB_IP="${FLATSAT_SUBNET}.64"

if [ ${system_flag} -eq 0 ]; then
    echo "Start Cosmos"
    docker run \
        -it \
        --rm \
        --net=host \
        -e DISPLAY \
        -e QT_X11_NO_MITSHM=1 \
        -e QT_GRAPHICSSYSTEM='native' \
        -e HOST_IP=${HOST_IP} \
        -e PDB_IP=${PDB_IP} \
        -e CDH_IP=${CDH_IP} \
        --user ${PUID}:${GUID} \
        -v ${CWD}:/cosmos \
        --name cosmos \
        has2/cosmos:latest

else 
    echo "Start COSMOS with system config file ${system_file} in ${CWD}"
    docker run \
        -it \
        --rm \
        --net=host \
        -e DISPLAY \
        -e QT_X11_NO_MITSHM=1 \
        -e HOST_IP=${HOST_IP} \
        -e PDB_IP=${PDB_IP} \
        -e CDH_IP=${CDH_IP} \
        --user ${PUID}:${GUID} \
        -v ${CWD}:/cosmos \
        --name cosmos \
        has2/cosmos:latest \
        "--system" "${system_file}"
fi