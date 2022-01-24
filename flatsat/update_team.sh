#!/usr/bin/bash
FLATSAT_REPO_PATH="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

if [ "$#" -ne 2 ]; then
    echo "Usage: ./update_team.sh <teamnum> <interface>"
    exit 1
fi

team_num_update=$1
ethernet_interface=$2
use_test_network=${3:-0}

echo "Updated team number: $team_num_update"
echo "Updated interface: $ethernet_interface"

export TEAM_NUMBER=$team_num_update
export SUBNET_BASE="192.168"
export TEAM_SUBNET_NUM=$(expr ${team_num_update} + 100)
export FLATSAT_SUBNET="${SUBNET_BASE}.${TEAM_SUBNET_NUM}"
export FLATSAT_SUBNET_MASK="${SUBNET_BASE}.${TEAM_SUBNET_NUM}.0/24"
export FLATSAT_GATEWAY="${SUBNET_BASE}.${TEAM_SUBNET_NUM}.1"
export HOST_IP="${FLATSAT_SUBNET}.2"
export SHIM_IP="${FLATSAT_SUBNET}.3"
export # COSMOS_IP="${FLATSAT_SUBNET}.3"
export FORTYTWO_IP="${FLATSAT_SUBNET}.100"
export DOCKER_NETWORK_IP_START="${FLATSAT_SUBNET}.128"
export MQTT_IP="${FLATSAT_SUBNET}.101"
export MQTT_GUI_IP="${FLATSAT_SUBNET}.102"
export ADCS_IP="${FLATSAT_SUBNET}.68"
export CDH_IP="${FLATSAT_SUBNET}.67"
export EDCL_DEFAULT_IP="192.168.0.51"
export EDCL_SETUP_IP="192.168.0.50"
export CDH_EDCL_IP="${FLATSAT_SUBNET}.5"
export PDB_IP="${FLATSAT_SUBNET}.64"
export COMM_IP="${FLATSAT_SUBNET}.65"
export CHALLENGER_TEAM="10.0.${TEAM_NUMBER}2.100"
export REPORTAPI_IP="10.0.${TEAM_NUMBER}1.100"
export REPORTAPI_GATEWAY="10.0.0.${TEAM_NUMBER}"
export DOCKER_NETWORK="flatsat${team_num_update}"
export ETHERNET_INTERFACE="${ethernet_interface}"
export TELEMETRY_DB_URL=$(cat postgres-credentials.json | jq ".team${TEAM_NUMBER}")
export USE_TEST_NETWORK=${use_test_network}
export BEELINK_IP="192.168.200.1${TEAM_NUMBER}"

echo "------------------------"
echo "Flatsat Data"
echo "FLAT_SAT_REPO=${FLATSAT_REPO_PATH}"
echo "TEAM: ${team_num_update}"
echo "Subnet: ${FLATSAT_SUBNET_MASK}"
echo "Local Ethernet Interface: ${ethernet_interface}"
echo "Docker Network: ${DOCKER_NETWORK}"
echo "Host IP (Cosmos): ${HOST_IP}"
echo "Host Shim IP (host <--> docker): ${SHIM_IP}"
echo "CDH EDCL (GRMON over Ethernet) IP: ${CDH_EDCL_IP}"
echo "42 IP: ${FORTYTWO_IP}"
echo "MQTT IP: ${MQTT_IP}"
echo "CDH IP: ${CDH_IP}"
echo "ADCS IP: ${ADCS_IP}"
echo "PDB IP: ${PDB_IP}"
echo "COMM IP: ${COMM_IP}"
echo "ETHERNET_INTERFACE: "${ethernet_interface}""
echo "------------------------"

if [ -f .env ]; then
    echo "Remove current .env file"
    rm .env
fi

echo "Create new .env file"
touch .env
cat << EOF > .env
DOCKER_USER_UID=$(id -u)
DOCKER_USER_GID=$(id -g)
TEAM_NUMBER=${TEAM_NUMBER}
SUBNET_BASE="${SUBNET_BASE}"
TEAM_SUBNET_NUM=${TEAM_SUBNET_NUM}
FLATSAT_SUBNET="${FLATSAT_SUBNET}"
FLATSAT_SUBNET_MASK="${FLATSAT_SUBNET_MASK}"
FLATSAT_GATEWAY="${FLATSAT_GATEWAY}"
HOST_IP="${HOST_IP}"
SHIM_IP="${SHIM_IP}"
FORTYTWO_IP="${FORTYTWO_IP}"
DOCKER_NETWORK_IP_START="${DOCKER_NETWORK_IP_START}"
MQTT_IP="${MQTT_IP}"
MQTT_GUI_IP="${MQTT_GUI_IP}"
ADCS_IP="${ADCS_IP}"
CDH_IP="${CDH_IP}"
EDCL_DEFAULT_IP="${EDCL_DEFAULT_IP}"
EDCL_SETUP_IP="${EDCL_SETUP_IP}"
CDH_EDCL_IP="${CDH_EDCL_IP}"
PDB_IP="${PDB_IP}"
COMM_IP="${COMM_IP}"
CHALLENGER_TEAM="${CHALLENGER_TEAM}"
REPORTAPI_IP="${REPORTAPI_IP}"
REPORTAPI_GATEWAY="${REPORTAPI_GATEWAY}"
DOCKER_NETWORK="${DOCKER_NETWORK}"
ETHERNET_INTERFACE="${ETHERNET_INTERFACE}"
TELEMETRY_DB_URL=${TELEMETRY_DB_URL}
USE_TEST_NETWORK=${USE_TEST_NETWORK}
BEELINK_IP="${BEELINK_IP}"
EOF

