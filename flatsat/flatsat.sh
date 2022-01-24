#!/usr/bin/bash


function exit_handler()
{
    echo "Shutting down flatsat.sh"
    ./stop.sh
    echo "FlatSat shutdown complete, exiting now."
    exit 0
}


RESET="\033[0m"
BOLD="\033[1m"
YELLOW="\033[38;5;11m"
RED="\033[31m"

usage="Usage: start.sh -t <team_num> -i <ethernet_interface>] -p (flag to program pdb) -c (flag to provission COMM payload) -f (flag to use Florida test range) -b (build cfs binaries) Starts support services for FLATSAT"
team_defined=0
team_num=1
ethernet_defined=0
ethernet_interface=""
build_cfs_defined=0
provision_pdb_defined=0
use_test_network=0
# provision_comm_defined=0

while getopts "h:t:i:pbf" option; 
do
    case "$option" in
        h|\?)
            echo ${usage};
            exit;;
        t )
            team_num=$OPTARG
            team_defined=1
            ;;
        i )
            ethernet_interface=$OPTARG
            ethernet_defined=1
            ;;
        f )
            use_test_network=1
            ;;
        p )
            provision_pdb_defined=1
            ;;
        b ) 
            build_cfs_defined=1
            ;;
        # c )
        #     provision_comm_defined=1
        #     ;;
        : )
            echo "Missing option argument for -$OPTARG" >&2; exit 1;;
        *  )
            echo "Unimplemented option: -$OPTARG" >&2; exit 1;;
    esac
done

if [ ${use_test_network} -eq 1 ]; then
    echo "Update enviroment to use FL Test Range Network"
    ./update_team.sh ${team_num} ${ethernet_interface} 1
fi


if [ ${team_defined} -eq 0 ]; then
    if [ -f .env ]; then
        echo "Source existing enviroment"
        source env.sh
        echo "Successfully sourced enviroment for TEAM: ${TEAM_NUMBER}"
    else
        echo -e $RED"Must define a team number -t <team_num>\n"$RESET
        echo ${usage};
        exit -1
    fi
fi

if [ ${team_defined} -eq 1 ] && [ ${ethernet_defined} -eq 1 ]; then
    ./update_team.sh ${team_num} ${ethernet_interface}
    source env.sh   
fi


if [ ${provision_pdb_defined} -eq 1 ]; then
    provision_pdb
    if [ $? -ne 0 ]; then
        echo "PDB Board Provisioning failed"
    fi
    exit
fi

# if [ ${provision_comm_defined} -eq 1 ]; then
#     update_comm
#     if [ $? -ne 0 ]; then
#         echo "COMM Payload Board Provisioning failed"
#     fi
# fi

if [ ${build_cfs_defined} -eq 1 ]; then
    build_cdh
    build_adcs
fi

if [ ${ethernet_defined} -eq 0 ]; then
    if [ ! -f .env ]; then 
        echo -e $RED"Must define a ethernet interface -e <ethernet_interface>. Run ifconfig to check available\n"$RESET
        echo ${usage};
        exit -1
    fi
fi

if [ -f .team{team_num} ]; then
    echo "FlatSat. Num ${team_num} already started. Stoping first before restarting."
    exit_handler
fi

touch .team${TEAM_NUMBER}

# echo "Update Docker Containers"
# docker-compose pull

echo "------------------------"
echo "------------------------"
echo "Setup Docker Network"

docker_network_exists=$(docker network ls | grep "${DOCKER_NETWORK}" | wc -l)

if [ ${docker_network_exists} -eq 0 ]; then

    netcmd=$(cat <<END_HEREDOC
docker network create -d ipvlan \
    --subnet ${FLATSAT_SUBNET_MASK} \
    --gateway ${FLATSAT_GATEWAY} \
    --ip-range ${DOCKER_NETWORK_IP_START}/27 \
    --aux-address host=${HOST_IP} \
    -o parent=${ETHERNET_INTERFACE} \
    ${DOCKER_NETWORK}
END_HEREDOC
)

    echo "${netcmd}"

    if ${netcmd}; then
        echo "Docker network ${DOCKER_NETWORK} created successfully"
    else 
        echo "Docker network startup failure"
        exit_handler
    fi

fi

echo "Check status of ethernet interface ${ETHERNET_INTERFACE}"

ethernet_exists=$(nmcli device status | grep "${ETHERNET_INTERFACE}" | wc -l )
if [ ${ethernet_exists} -eq 0 ]; then
    echo "Ethernet device ${ETHERNET_INTERFACE} not connected or doesn't exist"
    exit 1
fi

disconnected=$(nmcli device show ${ETHERNET_INTERFACE} | grep disconnected | wc -l)
if [ ${disconnected} -eq 1 ]; then
    echo "Link not connected, attempt to connect"
    nmcli device connect ${ETHERNET_INTERFACE}
fi


echo "Set IP to ${EDCL_SETUP_IP} for CDH board setup"
# nmcli device disconnect ${ETHERNET_INTERFACE}
# nmcli device modify ${ETHERNET_INTERFACE} ipv4.addresses "${COSMOS_IP}/24"
nmcli device modify ${ETHERNET_INTERFACE} ipv4.addresses "${EDCL_SETUP_IP}/24"
# nmcli device modify ${ETHERNET_INTERFACE} +ipv4.addresses 
# nmcli device modify ${ETHERNET_INTERFACE} ipv4.gateway "${FLATSAT_GATEWAY}"
nmcli device modify ${ETHERNET_INTERFACE} ipv4.method manual
# nmcli device connect ${ETHERNET_INTERFACE}

sleep 2

cdh eping ${EDCL_DEFAULT_IP}
edcl_status=$?
if [ ${edcl_status} -eq 0 ]; then
    cdh edcl-set-ip ${EDCL_DEFAULT_IP} ${CDH_EDCL_IP}
    echo "Switch EDCL IP from ${EDCL_DEFAULT_IP} to ${CDH_EDCL_IP} successful"
   
else
    echo "EDCL IP not default or not ready"
    cdh_ready=0
fi

sleep 2

echo "Set IP to ${HOST_IP} for FlatSat communication"
# nmcli device disconnect ${ETHERNET_INTERFACE}
nmcli device modify ${ETHERNET_INTERFACE} ipv4.addresses "${HOST_IP}/24"
nmcli device modify ${ETHERNET_INTERFACE} ipv4.gateway "${FLATSAT_GATEWAY}"
nmcli device modify ${ETHERNET_INTERFACE} ipv4.method manual
# nmcli device connect ${ETHERNET_INTERFACE}

sleep 2
cdh eping ${CDH_EDCL_IP}
edcl_status=$?
if [ ${edcl_status} -eq 0 ]; then
    echo "EDCL IP already set for current flatsat"
else
    echo "Could not verify C&DH EDCL IP, may need to restart"
fi

# We need a another macvlan bridge to allow us to connect to docker and flatsat subnet from host
shim_exists=$(nmcli device status | grep "${DOCKER_NETWORK}-shim" | wc -l )
if [ ${shim_exists} -eq 0 ]; then 
    echo "Create Local Shim Interface to Access Docker ipvlan network (not required with real routed network)"
    sudo ip link add "${DOCKER_NETWORK}-shim" link ${ETHERNET_INTERFACE} type ipvlan mode l2
    sudo ip addr add "${SHIM_IP}/32" dev "${DOCKER_NETWORK}-shim"
    sudo ip link set "${DOCKER_NETWORK}-shim" up
    # This route allows us to address containers by IP on docker network
    sudo ip route add "${FLATSAT_SUBNET}.96/27" dev "${DOCKER_NETWORK}-shim"
else
    echo "${DOCKER_NETWORK}-shim already exists, continue"
fi

echo "Update COMM Payload Software"
update_comm

echo "Update COMM Payload Challenge"
update_comm_challenge

echo "Update ADCS Softare"
copy_script_to_adcs
copy_to_adcs

echo "Start FlatSat Docker Services"
echo "docker-compose up -d"
docker-compose up -d

trap exit_handler SIGINT

terminator -g terminator_config -l FlatSat

echo "Exiting after terminator window was closed"
exit_handler

