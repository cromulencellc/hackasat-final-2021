#!/bin/bash

### Author: meenmachine
### Generates the challenge binary/keys and tarballs them for each team

REMOTE_DOCKER_TAG="has2/finals/comms/user-seg-client:latest"
LOCAL_DOCKER_TAG="user_segment:client"

RESET="\033[0m"
BOLD="\033[1m"
YELLOW="\033[38;5;11m"
RED="\033[31m"
GREEN="\033[0;32m"

NUM_TEAMS=8

if [ -d "./build" ]; then
    read -r -n 1 -p "$(echo -e $BOLD$YELLOW"build folder already exists. Delete? (y/n)? "$RESET)" reply
    echo ""
    if [[ "$reply" =~ ^(y|Y)$ ]]; then
        rm -rf build
    else
        echo "Exiting..."
        exit 0
    fi
fi

mkdir -p build/binary

echo -e "Building client binary..."
docker pull ${REMOTE_DOCKER_TAG}
docker run -it --rm \
    --user $(id -u):$(id -g) \
    -v $(pwd)/src/:/src/src \
    -v $(pwd)/include/:/src/include \
    -v $(pwd)/build/binary:/build/binary \
    ${REMOTE_DOCKER_TAG}

pushd build/binary/ > /dev/null
patchelf --replace-needed libssl.so.3 ./libs/libssl.so.3 ./client
patchelf --replace-needed libcrypto.so.3 ./libs/libcrypto.so ./client
popd > /dev/null
echo "Done"

for i in $(seq 1 $NUM_TEAMS)
do
    TEAM_PRIV_FILE="../server/configs/team_private_keys/team_${i}_rsa_priv.pem"
    if [ ! -f $TEAM_PRIV_FILE ]; then
        echo -e $YELLOW"Could not find team ${i}'s private key file"
        echo -e "Attempting to generate myself..."$RESET
        pushd ../server/ > /dev/null
        make keys_only
        if [ $? -eq 0 ] && [ -f $TEAM_PRIV_FILE ]; then
            echo -e $GREEN"Generated them successfully"$RESET
            popd > /dev/null
            continue
        else
            echo -e $RED"Team ${i}'s private key file (${TEAM_PRIV_FILE}) DOES NOT exist"
            echo "And FAILED generation on my own."
            echo "Please generate the private key file by running server.py"
            echo -e "Bailing out..."$RESET
            exit -1
        fi
    fi
done

TEAM_ATTR_KEYS_PATH="../server/configs/team_attr_keys.yml"
if [ ! -f $TEAM_ATTR_KEYS_PATH ]; then
    echo -e $YELLOW"Could not find team attribution keys"
    echo -e "Attempting to generate myself..."$RESET
    pushd ../server/ > /dev/null
    make keys_only

    if [ $? -eq 0 ] && [ -f $TEAM_ATTR_KEYS_PATH ]; then
        echo -e $GREEN"Generated them successfully"$RESET
        popd > /dev/null
    else
        echo -e $RED"Teams attribution key file (${TEAM_ATTR_KEYS_PATH}) DOES NOT exist"
        echo "And FAILED generation on my own."
        echo "Please generate the attribution keys file by running server.py"
        echo -e "Bailing out..."$RESET
        exit -1
    fi
fi

for i in $(seq 1 $NUM_TEAMS)
do
    TEAM_PRIV_FILE=../server/configs/team_private_keys/team_${i}_rsa_priv.pem
    TEAM_PUB_FILE=../server/configs/team_public_keys/team_${i}_rsa_pub.pem
    TEAM_BUILD_PATH=build/team_files/team_${i}
    TEAMS_BUILD_PATH=build/team_files/

    echo -n "Copying team ${i} client files and keys... "
    mkdir -p ${TEAM_BUILD_PATH}/client
    mkdir -p ${TEAM_BUILD_PATH}/keys

    # Copy team public and private key files
    cp $TEAM_PRIV_FILE $TEAM_BUILD_PATH/keys/
    cp $TEAM_PUB_FILE $TEAM_BUILD_PATH/keys/

    # Copy client binary
    cp -r build/binary/* ${TEAM_BUILD_PATH}/client

    # Awk out single line from attribution_keys and put it in attribution_key.txt
    awk -v team_num="$i" 'NR==team_num' ../server/configs/team_attr_keys.yml > $TEAM_BUILD_PATH/team_${i}_attribution_key.txt
    
    pushd $TEAMS_BUILD_PATH > /dev/null
    tar -czf team_${i}.tar.gz team_${i}
    popd > /dev/null

    echo "Done"
done

echo "Done"
