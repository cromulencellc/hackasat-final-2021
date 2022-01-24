#!/bin/bash

TEAM=${TEAM_NUMBER:-1}

./has2_env.sh

MQTT_ENDPOINT=${MQTT_IP:-localhost}
echo "HAS2 42 Simulation for Team ${TEAM} /w MQTT: ${MQTT_ENDPOINT}"

# sed -i "s/Orb_TEAM_[0-9].txt/Orb_TEAM_${TEAM}.txt/" HAS2/Inp_Sim.txt

# echo "Update MQTT Host for Docker"
# sed -i "s/localhost/${MQTT_ENDPOINT}/" HAS2/Inp_IPC.txt

echo "Starting 42 for Team ${TEAM}"

if [ ! -d ./HAS2 ]; then
    echo "HAS2 Configuration Folder Not Mounted"
    exit 1
fi

./42 HAS2