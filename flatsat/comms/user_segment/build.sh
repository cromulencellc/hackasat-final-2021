#!/bin/bash

RESET="\033[0m"
BOLD="\033[1m"
YELLOW="\033[38;5;11m"
RED="\033[31m"

DEPS="build-essential python3-pip"

echo -e $BOLD"Build User Segment\n"$RESET

usage="Usage: build.sh [-l (local build. No docker) -d (build docker images locally) -h (help)]"

local_build=0
docker_build=0
while getopts "h:ld" option;
do
    case "$option" in
        h|\?)
            echo ${usage};
            exit;;
        l )
            local_build=1;;
	    d )
            docker_build=1;;
    esac
done

dpkg -l $DEPS &> /dev/null
if [ $? -eq 1 ]; then
    echo -e $RED"Dependencies not installed. Installing now..."$RESET
    sudo -v
    sudo apt install -y $DEPS
fi

if [ ${local_build} -eq 1 ]; then
    echo -e $RED"Can't do this anymore. Bailing out..."$RESET
    exit -1
    # Compile openssl
    if [ -d "openssl/build" ]; then
        echo -e $YELLOW"Openssl is already compiled. Skipping... (if not, delete 'user/openssl/build' and rerun).\n"$RESET
    else
        echo -e $YELLOW"Compiling openssl\n"$RESET
        git submodule update --init
        mkdir -p openssl/build
        pushd openssl/build > /dev/null
        ../config
        make -j8
        popd > /dev/null
    fi

    # Make client
    pushd client > /dev/null
    echo -e $YELLOW"Compiling client"$RESET
    make local_build
    popd > /dev/null

    # Make User Segment
    pushd server > /dev/null
    pushd tools/key_gen > /dev/null
    echo -e $YELLOW"Compiling user segment server keygen"$RESET
    make local_build
    popd > /dev/null
    echo -e $YELLOW"\nMaking server python virtual enviroment"$RESET
    ./build_env.sh
    popd > /dev/null

    echo -e "\nRun 'source activate' in the 'server', then run ./server.py"
elif [ ${docker_build} -eq 1 ]; then
    echo -e $YELLOW"Building docker images locally"$RESET

    # Make Client Docker Image
    pushd client > /dev/null
    make docker_build
    popd > /dev/null

    # Make User Segment Docker Image
    pushd server > /dev/null
    make docker_build
    popd > /dev/null

    # Make ping-controller Docker Image
    pushd server/tools/ping_controller/ > /dev/null
    make docker_build
    popd > /dev/null
else
    # Make Client Docker Image
    pushd client > /dev/null
    make docker_pull
    popd > /dev/null

    # Make User Segment Docker Image
    pushd server > /dev/null
    make docker_pull
    popd > /dev/null

    # Make ping-controller Docker Image
    pushd server/tools/ping_controller/ > /dev/null
    make docker_pull
    popd > /dev/null
fi
