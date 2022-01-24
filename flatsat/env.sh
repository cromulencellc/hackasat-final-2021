#!/usr/bin/bash
FLATSAT_REPO_PATH="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Guard the script against execution - it must be sourced!
echo $0 | egrep 'setup_env.sh' > /dev/null
if [ $? -eq 0 ]; then
    echo ""
    echo "ERROR: the setup file must be SOURCED, NOT EXECUTED in a shell."
    echo "Try (for bash)  : source setup_env.sh"
    echo "Or (eg. for ksh): . setup_env.sh"
    exit 1
fi

# echo "Source team enviroment"
if [ ! -f .env ]; then
    echo "Run flatsat.sh -t <team_num> to generate team enviroment"
    exit
fi

source .env

source ./board_tools/env.sh

export TEAM_NUMBER=${TEAM_NUMBER}
export SUBNET_BASE="${SUBNET_BASE}"
export TEAM_SUBNET_NUM=${TEAM_SUBNET_NUM}
export FLATSAT_SUBNET="${FLATSAT_SUBNET}"
export FLATSAT_SUBNET_MASK="${FLATSAT_SUBNET_MASK}"
export FLATSAT_GATEWAY="${FLATSAT_GATEWAY}"
export HOST_IP="${HOST_IP}"
export SHIM_IP="${SHIM_IP}"
export FORTYTWO_IP="${FORTYTWO_IP}"
export DOCKER_NETWORK_IP_START="${DOCKER_NETWORK_IP_START}"
export MQTT_IP="${MQTT_IP}"
export MQTT_GUI_IP="${MQTT_GUI_IP}"
export ADCS_IP="${ADCS_IP}"
export CDH_IP="${CDH_IP}"
export EDCL_DEFAULT_IP="${EDCL_DEFAULT_IP}"
export EDCL_SETUP_IP="${EDCL_SETUP_IP}"
export CDH_EDCL_IP="${CDH_EDCL_IP}"
export PDB_IP="${PDB_IP}"
export COMM_IP="${COMM_IP}"
export CHALLENGER_TEAM="${CHALLENGER_TEAM}"
export DOCKER_NETWORK="${DOCKER_NETWORK}"
export ETHERNET_INTERFACE="${ETHERNET_INTERFACE}"

get_new_env() {
    source env.sh;
}

get_new_shell() {
    /bin/bash
}

load_flags() {
    echo "Loading Game Flags from .flags"
    source .flags
}

kill_adcs() {
    echo "Process kill running instances of ADCS software"
    ssh -o "UserKnownHostsFile=/dev/null" -o "StrictHostKeyChecking=no" adcs@${ADCS_IP} "pgrep core-cpu1 | xargs kill -9";
}

ssh_adcs() {
    ssh -o "UserKnownHostsFile=/dev/null" -o "StrictHostKeyChecking=no" adcs@${ADCS_IP} -t "cd /apps/cpu1; bash --login";
}

adcs_challenge_deposit_flag() {
	cd ~/flatsat/
	echo "Updating ADCS challenge (team: $TEAM_NUMBER) with flag $1"
	printf "$1" > adcs/challenge/flag.txt
	scp adcs/challenge/flag.txt adcs@$ADCS_IP:/apps/cpu1/cf/
	echo "Restarting challenge"
	# Send cosmos command to update ADCS challenge
	python3 -c "import ballcosmos;ballcosmos.commands.cmd('ADCS UPDATE_FLAG with CCSDS_STREAMID 6623, CCSDS_SEQUENCE 49152, CCSDS_LENGTH 2, CCSDS_FUNCCODE 14, CCSDS_CHECKSUM 0, UPDATE UPDATE')"

	echo "Deleting flag file"
	ssh -t adcs@$ADCS_IP "rm /apps/cpu1/cf/flag.txt"
}

start_adcs() {
    kill_adcs
    sleep 10
    ssh -o "UserKnownHostsFile=/dev/null" -o "StrictHostKeyChecking=no" -t adcs@${ADCS_IP} "/bin/bash -ic 'cd /apps/cpu1; ./core-cpu1;'";
}

copy_challenge_to_adcs () {
    echo "Copy Updated ADCS Build to ADCS ${ADCS_IP}"
    echo "Check for valid build at: ${FLATSAT_REPO_PATH}/opensatkit/cfs/build_osk_adcs_challenge/exe/cpu1"
    if [ -d "${FLATSAT_REPO_PATH}/opensatkit/cfs/build_osk_adcs_challenge/exe/cpu1" ]; then
        echo "rsync -avzh --delete \
            ${FLATSAT_REPO_PATH}/opensatkit/cfs/build_osk_adcs_challenge/exe/cpu1/ \
            adcs@${ADCS_IP}:/apps/cpu1"
        rsync -avzh --delete \
            ${FLATSAT_REPO_PATH}/opensatkit/cfs/build_osk_adcs_challenge/exe/cpu1/ \
            adcs@${ADCS_IP}:/apps/cpu1
        # scp -r "${FLATSAT_REPO_PATH}/opensatkit/cfs/build_osk_adcs/exe/cpu1" adcs@${ADCS_IP}:/apps/
    fi
}

restart_adcs_core (){
    echo "Sending SIGINT to running instances of ADCS software"
    ssh -o "UserKnownHostsFile=/dev/null" -o "StrictHostKeyChecking=no" adcs@${ADCS_IP} "pgrep core-cpu1 | xargs kill -2";
}

restart_cdh_core () {
    pgrep sparc-rtems5 | xargs kill -2
}

restore_cdh () {
    if [ -d ${FLATSAT_REPO_PATH}/opensatkit/cfs/build_osk_cdh.save ]; then
        mv ${FLATSAT_REPO_PATH}/opensatkit/cfs/build_osk_cdh ${FLATSAT_REPO_PATH}/opensatkit/cfs/build_osk_cdh_challenge
        mv ${FLATSAT_REPO_PATH}/opensatkit/cfs/build_osk_cdh.save ${FLATSAT_REPO_PATH}/opensatkit/cfs/build_osk_cdh
    else
        echo "Error: !! Everything is already restored. You should check yourself."
    fi
}

start_cdh_challenge () {
    echo ${FLATSAT_REPO_PATH}/opensatkit/cfs/build_osk_cdh
    if [ -d ${FLATSAT_REPO_PATH}/opensatkit/cfs/build_osk_cdh.save ]; then
        echo "Exploitable binary folder already active. Skipping renaming..."
    else
        mv ${FLATSAT_REPO_PATH}/opensatkit/cfs/build_osk_cdh ${FLATSAT_REPO_PATH}/opensatkit/cfs/build_osk_cdh.save
        mv ${FLATSAT_REPO_PATH}/opensatkit/cfs/build_osk_cdh_challenge ${FLATSAT_REPO_PATH}/opensatkit/cfs/build_osk_cdh
    fi

    sleep 1
    restart_cdh_core
}

start_adcs_challenge () {
    copy_challenge_to_adcs
    sleep 1
    restart_adcs_core
}

copy_to_adcs () {
    kill_adcs
    echo "Copy Updated ADCS Build to ADCS ${ADCS_IP}"
    echo "Check for valid build at: ${FLATSAT_REPO_PATH}/opensatkit/cfs/build_osk_adcs/exe/cpu1"
    if [ -d "${FLATSAT_REPO_PATH}/opensatkit/cfs/build_osk_adcs/exe/cpu1" ]; then
        echo "rsync -avzh --delete \
            ${FLATSAT_REPO_PATH}/opensatkit/cfs/build_osk_adcs/exe/cpu1/ \
            adcs@${ADCS_IP}:/apps/cpu1"
        rsync -avzh --delete \
            ${FLATSAT_REPO_PATH}/opensatkit/cfs/build_osk_adcs/exe/cpu1/ \
            adcs@${ADCS_IP}:/apps/cpu1
    fi
}

kill_adcs_script() {
    echo "Process kill running instances of ADCS software"
    ssh -o "UserKnownHostsFile=/dev/null" -o "StrictHostKeyChecking=no" adcs@${ADCS_IP} "pgrep forever_adcs.sh | xargs kill -9";
}

start_adcs_script() {
    kill_adcs
    kill_adcs_script
    sleep 10
    ssh -o "UserKnownHostsFile=/dev/null" -o "StrictHostKeyChecking=no" -t adcs@${ADCS_IP} "/bin/bash -ic 'cd /apps/; ./forever_adcs.sh;'";
}

copy_script_to_adcs() {
    kill_adcs
    kill_adcs_script
    echo "Copying forever core script to adcs at ${ADCS_IP}"
    echo "rsync -avzh --delete \
        ${FLATSAT_REPO_PATH}/scripts/forever_adcs.sh \
        adcs@${ADCS_IP}:/apps/"
    rsync -avzh --delete \
        ${FLATSAT_REPO_PATH}/scripts/forever_adcs.sh \
        adcs@${ADCS_IP}:/apps/
}

build_cdh() {
    echo "Build CDH"
    startDir=$(pwd)
    cd "${FLATSAT_REPO_PATH}/opensatkit/cfs"
    ./build.sh -m osk_cdh -t ${TEAM_NUMBER}
    if [ $? -ne 0 ]; then
        echo "CDH Build Failed, exit"
        exit 1
    fi
    cd ${startDir}
}

start_cdh_gdb() {
    opensatkit/cfs/debugger/sparc-gdb.sh
}

start_cdh_serial_console() {
    miniterm /dev/ttyUSB0 38400
}

start_cdh_edcl_grmon() {
    cdh edebug ${CDH_EDCL_IP};
}

comm_challenge_deposit_flag() {
	cd ~/flatsat/
	echo "Updating comm challenge (team: $TEAM_NUMBER) with flag $1"
	printf "export FLAG1=$1\nexport FLAG2=$1\nexport FLAG3=$1\nexport FLAG4=$1\nexport FLAG5=$1\nexport FLAG6=$1\nexport FLAG7=$1\nexport FLAG8=$1\n" > comms/challenge/flags.txt
	scp comms/challenge/flags.txt comm@$COMM_IP:flags.txt
    	echo "Restarting challenge"
	comm_challenge_start
}

comm_challenge_update() {
    echo "Updating challenge on COMM payload"
    if [ ! -f comms/challenge/comms-challenge-arm64.tar ]; then
        echo "Can't find comms-challenge-arm64.tar docker export"
        echo "Save updated comms-challenge-arm64 to tar file"
        docker save -o comms/challenge/comms-challenge-arm64.tar has2/finals/comms/comms-challenge-arm64:latest
    fi
    if [ ! -f comms/challenge/telemetry/comms-challenge-telemetry-arm64.tar ]; then
        echo "Can't find comms-challenge-telemetry-arm64.tar docker export"
        echo "Save updated comms-challenge-telemetry-arm64 to tar file"
        docker save -o comms/challenge/telemetry/comms-challenge-telemetry-arm64.tar has2/finals/comms/comms-challenge-telemetry-arm64:latest
    fi

    echo "Pulled challenge and telemetry docker containers and created tarballs"

    challenge_cmd="rsync -avzh --progress \
        --stats \
        comms/challenge/comms-challenge-arm64.tar comms/challenge/telemetry/comms-challenge-telemetry-arm64.tar \
        comm@${COMM_IP}:/home/comm/"
    echo "${challenge_cmd}"
    challenge_updated=$(${challenge_cmd} | grep 'files transferred' | cut -f2 -d':' | xargs)
    echo "Comm challenge Updated: ${challenge_updated}"
    challenge_cmd="rsync -avzh --progress \
        --stats \
        comms/challenge/docker-compose.yml comms/challenge/flags.txt \
        comm@${COMM_IP}:/home/comm/"
    echo "${challenge_cmd}"
    challenge_updated=$(${challenge_cmd} | grep 'files transferred' | cut -f2 -d':' | xargs)
    echo "Comm challenge telemetry script updated: ${challenge_updated}"

    echo "Load COMM Payload challenge Docker into local COMM challenge docker repository"
    ssh comm@${COMM_IP} 'docker load -i /home/comm/comms-challenge-arm64.tar && docker load -i /home/comm/comms-challenge-telemetry-arm64.tar && docker system prune -f'

}


update_comm () {
    get_new_env
    load_flags
    echo "Update softwre on COMM Payload Linux"
    startDir=$(pwd)
    echo "rsync COMM Payload GNU Radio Flowgraphs to COMM Payload"
    ssh comm@${COMM_IP} 'mkdir -p /home/comm/payload/flowgraphs/'
    flatsat_rsync_cmd="rsync -avh --delete \
        --progress \
        --stats \
        --exclude payload_flatsat_rx.py \
        comms/payload/radio/flowgraphs/flatsat/ \
        comm@${COMM_IP}:/home/comm/payload/flowgraphs/flatsat"
    echo "${flatsat_rsync_cmd}"
    flatsat_flowgraph_updated=$(${flatsat_rsync_cmd} | grep 'files transferred' | cut -f2 -d':' | xargs )
    shared_rsync_cmd="rsync -avh --delete \
        --progress \
        --stats \
        --exclude usersegment_tx.grc \
        comms/shared/flowgraphs/dependencies/ \
        comm@${COMM_IP}:/home/comm/payload/flowgraphs/dependencies"
    echo "${shared_rsync_cmd}"
    shared_flowgraph_updated=$(${shared_rsync_cmd} | grep 'files transferred' | cut -f2 -d':' | xargs)
    echo "Compile payload GNU Radio Flowgraphs"
    echo "Flatsat Flowgraphs Updated: ${flatsat_flowgraph_updated}"
    echo "Shared Flowgraphs Updated: ${shared_flowgraph_updated}"
    if [ ${shared_flowgraph_updated} -gt 0 ]; then
        echo "Compile shared flowgraphs"
        ssh comm@${COMM_IP} "cd /home/comm/payload/flowgraphs/dependencies && grcc *.grc"
    fi
    if [ ${flatsat_flowgraph_updated} -gt 0 ]; then
        echo "Compile flatsat flowgraph"
        ssh comm@${COMM_IP} "cd /home/comm/payload/flowgraphs/flatsat && grcc *.grc"
    fi
    echo "rsync volk_profile"
    rsync -avh --delete \
        --progress \
        comms/payload/radio/volk/ \
        comm@${COMM_IP}:/home/comm/.volk
    # ssh comm@${COMM_IP} "cd /home/comm/payload/flowgraphs/dependencies && grcc *.grc && cd /home/comm/payload/flowgraphs/flatsat && grcc *.grc"
    echo "rsync COMM Payload Dispatcher Docker container to COMM Payload"
    if [ ! -f comms/payload/payload-dispatcher-arm64.tar ]; then
        echo "Can't find payload-dispatcher docker export"
        docker save -o comms/payload/payload-dispatcher-arm64.tar has2/finals/comms/payload-dispatcher-arm64:latest
    fi
    dispatcher_cmd="rsync -avzh --progress \
        --stats \
        comms/payload/payload-dispatcher-arm64.tar \
        comm@${COMM_IP}:/home/comm/payload/dispatcher/"
    echo "${dispatcher_cmd}"
    dispatcher_updated=$(${dispatcher_cmd} | grep 'files transferred' | cut -f2 -d':' | xargs)
    echo "Dispatcher Updated: ${dispatcher_updated}"
    if [ ${dispatcher_updated} -gt 0 ]; then
        echo "Load COMM Payload Dispatcher Docker into local COMM payload docker repository"
        ssh comm@${COMM_IP} 'docker load -i /home/comm/payload/dispatcher/payload-dispatcher-arm64.tar && docker system prune -f'
    fi
    echo "Copy COMM Payload Dispatcher Start Script to COMM payload"
    cp comms/payload/run_dispatcher.sh comms/payload/run_dispatcher.sh.bak
    sed -i "s/@MQTT_IP_ADDR@/${MQTT_IP}/" comms/payload/run_dispatcher.sh
    sed -i "s/@PAYLOAD_DISPATCHER_FLAG@/${PAYLOAD_DISPATCHER_FLAG}/" comms/payload/run_dispatcher.sh
    scp comms/payload/run_dispatcher.sh comm@${COMM_IP}:/home/comm/payload/
    mv comms/payload/run_dispatcher.sh.bak comms/payload/run_dispatcher.sh
    
    echo "COMM Payload Software update complete"
    cd ${startDir}
}


comm_payload_start() {
    ssh -o "UserKnownHostsFile=/dev/null" -o "StrictHostKeyChecking=no" -t comm@${COMM_IP} "/usr/bin/bash -ic 'cd /home/comm/payload/flowgraphs/flatsat && ./payload_flatsat_rx.py --team-num ${TEAM_NUMBER}'"
}

comm_dispatcher_start() {
    ssh -o "UserKnownHostsFile=/dev/null" -o "StrictHostKeyChecking=no" -t comm@${COMM_IP} "/usr/bin/bash -ic 'MQTT_IP=${MQTT_IP} TEAM_NUMBER=${TEAM_NUMBER} /home/comm/payload/run_dispatcher.sh'";
}

comm_dispatcher_stop() {
    ssh -o "UserKnownHostsFile=/dev/null" -o "StrictHostKeyChecking=no" -t comm@${COMM_IP} "/usr/bin/bash -ic 'pgrep -f run_dispatcher.sh | xargs kill'";
}

comm_challenge_start() {
    ssh -o "UserKnownHostsFile=/dev/null" -o "StrictHostKeyChecking=no" -t comm@${COMM_IP} "/usr/bin/bash -ic 'source flags.txt && docker-compose down && MQTT_IP=${MQTT_IP} docker-compose up -d'";
}

comm_challenge_logs() {
    api_cmd="docker-compose logs -f"
    echo "Viewing docker logs on comm board: ${api_cmd}"
    ssh -t comm@${COMM_IP} ${api_cmd}	
}

comm_challenge_stop() {
    ssh -o "UserKnownHostsFile=/dev/null" -o "StrictHostKeyChecking=no" -t comm@${COMM_IP} "/usr/bin/bash -ic 'source flags.txt && docker-compose down'";
}

comm_ssh() {
    ssh -o "UserKnownHostsFile=/dev/null" -o "StrictHostKeyChecking=no" -t comm@${COMM_IP} "/usr/bin/bash -i"
}

comm_clean() {
        cat << EOF > /tmp/clean_comm.sh
#!/usr/bin/bash
echo "Cleaning comm: $(date)"
#Remove user bash, vim etc histories
sudo rm /home/comm/.bash_history
sudo rm /home/comm/.viminfo
#Clean challenge and exploit history???
#sudo rm /home/comm/exploit*
#sudo rm /home/comm/flag*
#sudo rm /home/comm/comm-challenge*
#Should all of this stuff by on commadm not comm??
EOF
    chmod +x /tmp/clean_comm.sh
    scp /tmp/clean_comm.sh "commadm@${COMM_IP}:/home/commadm/"
    ssh -t commadm@${COMM_IP} "/home/commadm/clean_comm.sh | tee -a /home/commadm/clean_comm.log"
}

adcs_ssh() {
    ssh adcs@${ADCS_IP}
}

adcs_clean() {
        cat << EOF > /tmp/clean_adcs.sh
#!/bin/bash
echo "Cleaning adcs: $(date)"
#Remove user bash, vim etc histories
sudo rm /home/adcs/.bash_history
sudo rm /home/adcs/.viminfo
EOF
    chmod +x /tmp/clean_adcs.sh
    scp /tmp/clean_adcs.sh "adcsadm@${ADCS_IP}:/home/adcsadm/"
    ssh -t adcsadm@${ADCS_IP} "/home/adcsadm/clean_adcs.sh | tee -a /home/adcsadm/clean_adcs.log"
}

api_ssh() {
    ssh -o "UserKnownHostsFile=/dev/null" -o "StrictHostKeyChecking=no" -t hasadmin@${REPORTAPI_IP} "/usr/bin/bash -i"
}

tail_mosquitto_logs() {
    docker-compose logs -f -t mosquitto
}

tail_fortytwo_logs() {
    docker-compose logs -f -t fortytwo
}

tail_fortytwobridge_logs() {
    docker-compose logs -f -t fortytwo-bridge
    
}

tail_cosmos_logs() {
    docker-compose logs -f -t cosmos;
}
