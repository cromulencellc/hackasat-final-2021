#!/bin/bash

## Main Script
echo "CFS Build Mission Binaries"

# declare -A cpu_target_map=( ["cpu1"]="TGT1" ["cpu2"]="TGT2" ["cpu3"]="TGT3" ["cpu4"]="TGT4")

usage="Usage: build_mission.sh <mission_name> <incremental_build> <challenge_build>"

if [ $# -lt 2 ]; then
    echo "Incorrect usage"
    echo ${usage}
    exit 1
fi

declare -a missions_all=("osk" "osk_rtems" "osk_cdh" "osk_adcs")

mission=$1
incremental_build=$2

echo "build_mission.sh -> Mission ${mission}, Incremental ${incremental_build}"

# curDir=$(pwd)
# echo "Current Directory ${curDir}"
# echo "Directory Listing"
# ls -l

if [ -z ${mission} ]; then
    echo "Mission not set, exiting..."
    exit 1
fi

export ATTRIBUTION_KEY=$(awk -v team_num="$TEAM_NUM" 'NR==team_num {print $2}' team_attr_keys.yml)
echo "ATTRIBUTION_KEY=${ATTRIBUTION_KEY}"

export O="build_${mission}"
export MISSIONCONFIG=${mission}

DEFS_FOLDER="${MISSIONCONFIG}_defs"

echo "Pre Build Tasks"
TEAM_SUBNET_NUM=$(expr ${TEAM_NUM} + 100)
MQTT_IP="192.168.${TEAM_SUBNET_NUM}.101"
echo "Update MQTT IP Address to ${MQTT_IP}"
sed -i.bak "s/@MQTT_IP_ADDR@/${MQTT_IP}/" ${DEFS_FOLDER}/cpu1_mqtt_ini.json
if [ $? -ne 0 ]; then
    echo "MQTT IP Update error"
    exit 1
fi

# If incremental build is false run clean for full build
if [ ${incremental_build} -eq 0 ]; then
    echo "Clean & Prep"
    make distclean

    # Pre-build mission specific build steps here
    if [ "${mission}" == "osk_cdh" ]; then
        echo "Create eeprom-tar.c stub at psp/fsw/leon3-rtems/src/eeprom-tar.c"

cat << EOF > psp/fsw/leon3-rtems/src/eeprom-tar.c
/*
*  Declarations for C structure representing binary file eeprom-tar
*
*  WARNING: Automatically generated -- do not edit!
*/

#include <sys/types.h>

const unsigned char eeprom_tar[] = {0};

const size_t eeprom_tar_size = sizeof(eeprom_tar);
EOF
        if [ ! -f psp/fsw/leon3-rtems/src/eeprom-tar.c ]; then
            echo "eeprom-tar.c stub creation failed"
            exit 1
        fi
    fi

    make prep
fi

# Always run install
make install

# Mission specific build steps here
if [ "${mission}" == "osk_cdh" ]; then
    pushd "${O}/exe/cpu1"
    tar cf eeprom-tar eeprom/
    rtems-bin2c eeprom-tar eeprom-tar
    cp eeprom-tar.h /apps/opensatkit/cfs/psp/fsw/leon3-rtems/inc/eeprom-tar.h
    cp eeprom-tar.c /apps/opensatkit/cfs/psp/fsw/leon3-rtems/src/eeprom-tar.c
    popd
    make install
fi

build_successful=$?

echo "Post Build Tasks"
echo "Revert MQTT INI File"
mv ${DEFS_FOLDER}/cpu1_mqtt_ini.json.bak ${DEFS_FOLDER}/cpu1_mqtt_ini.json
if [ "${mission}" == "osk_adcs" ]; then
    if [ -f ${O}/exe/cpu1/cf/implant.so ]; then
        mv ${O}/exe/cpu1/cf/implant.so ${O}/exe/
    fi
fi

if [ -${build_successful} -ne 0 ]; then
    echo "Build Failed for ${mission}"
    exit $build_successful
fi


exit $build_successful