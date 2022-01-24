#!/bin/bash
competitor_targets=(ADCS CF CF2 CFDP CFE_ES CFE_ES2 CFE_EVS CFE_EVS2 CFE_SB CFE_TBL CFE_TIME CFS_KIT COMM COSMOS CS DS EPHEM EPS_MGR FM FM2 HK HS KIT_CI KIT_SCH KIT_TO LC MD MM MQTT MQTT2 PDU PDU2 PL_IF SBN_LITE SBN_LITE2 SC SLA_TLM SYSTEM )
api_targets=(ADCS COSMOS SLA_TLM EPS_MGR COMM SYSTEM)

create_challenger_deployment() {

    cd ${COSMOS_PATH}
    team=$1
    output=$2
    flatsat_sff_ip=$3

    team_dir="${output}/team_${team}/challenger"
    team_cosmos="${team_dir}/cosmos"

    if [ -d ${team_cosmos} ]; then
        echo "Remove existing team dir ${team_cosmos} to ensure clean deployment"
        rm -rf ${team_cosmos}
    fi

    team_cosmos="${team_dir}/cosmos"

    echo "Create team output cosmos folder ${team_cosmos}"
    mkdir -p ${team_cosmos}

    echo "Copy Files for Deployment"

    echo "cp -rv cfs_kit/ ${team_cosmos}/"
    cp -rv cfs_kit/ ${team_cosmos}/

    echo "cp -rv outputs/ ${team_cosmos}/"
    cp -rv outputs/ ${team_cosmos}/

    echo "cp -rv tools/ ${team_cosmos}/"
    cp -rv tools/ ${team_cosmos}/

    echo "cp -rv scripts/ ${team_cosmos}/"
    cp -rv scripts/ ${team_cosmos}/

    echo "mkdir -p procedures/ ${team_cosmos}/"
    mkdir -p procedures/ ${team_cosmos}/

    echo "cp -rv lib/ ${team_cosmos}/"
    cp -rv lib/ ${team_cosmos}/

    nextcmd="rm ${team_cosmos}/lib/mqtt_job.rb"
    echo "$nextcmd"
    ${nextcmd}

    echo "Remove telemtry database library ruby file"
    echo "rm ${team_cosmos}/lib/telemetry_db.rb"
    rm ${team_cosmos}/lib/telemetry_db.rb

    echo "Copy files in cosmos folder"
    echo "find . -maxdepth 1 -type f -exec cp \"{}\" ${team_cosmos}/ \;"
    find . -maxdepth 1 -type f -exec cp "{}" ${team_cosmos}/ \;

    echo "Create output downloads folder"
    echo "mkdir -p ${team_cosmos}/downloads"
    mkdir -p ${team_cosmos}/downloads

    echo "Create output config folder"
    echo "mkdir -p ${team_cosmos}/config"
    mkdir -p ${team_cosmos}/config
    echo "cp -rv config/data ${team_cosmos}/config/"
    cp -rv config/data ${team_cosmos}/config/
    echo "cp -rv config/system ${team_cosmos}/config/"
    cp -rv config/system ${team_cosmos}/config/

    echo "cp -rv config/tools ${team_cosmos}/config/"
    cp -rv config/tools ${team_cosmos}/config/

    echo "Remove Config Files Not Needed for Team"
    echo "rm ${team_cosmos}/config/tools/launcher/launcher_monitor.txt"
    echo "rm ${team_cosmos}/config/tools/launcher/launcher_testbed.txt"
    echo "rm ${team_cosmos}/config/tools/launcher/launcher.txt"
    rm "${team_cosmos}/config/tools/launcher/launcher_monitor.txt"
    rm "${team_cosmos}/config/tools/launcher/launcher_testbed.txt"
    rm "${team_cosmos}/config/tools/launcher/launcher.txt"
    rm "${team_cosmos}/config/tools/launcher/launcher_api.txt"
    rm "${team_cosmos}/Launcher_api.txt"
    
    rm "${team_cosmos}/config/system/system_challenge.txt"
    rm "${team_cosmos}/config/system/system.txt"
    rm "${team_cosmos}/config/tools/cmd_tlm_server/cmd_tlm_server_digitaltwin_testbed.txt"
    rm "${team_cosmos}/config/tools/cmd_tlm_server/cmd_tlm_server_digitaltwin.txt"
    rm "${team_cosmos}/config/tools/cmd_tlm_server/cmd_tlm_server_pdb.txt"
    rm "${team_cosmos}/config/tools/cmd_tlm_server/cmd_tlm_server.txt"
    rm "${team_cosmos}/config/tools/cmd_tlm_server/cmd_tlm_server_api.txt"
    rm "${team_cosmos}/config/tools/cmd_tlm_server/cmd_tlm_server_challenge.txt"
    rm "${team_cosmos}/config/system/system_api.txt"
    rm "${team_cosmos}/run.sh"

    echo "Rename files for competitor distribution"
    mv "${team_cosmos}/config/tools/cmd_tlm_server/cmd_tlm_server_competitor.txt" "${team_cosmos}/config/tools/cmd_tlm_server/cmd_tlm_server.txt"
    echo "mv ${team_cosmos}/config/tools/launcher/launcher_competitor.txt ${team_cosmos}/config/tools/launcher/launcher.txt"
    mv "${team_cosmos}/config/tools/launcher/launcher_competitor.txt" "${team_cosmos}/config/tools/launcher/launcher.txt"
    echo "mv ${team_cosmos}/config/system/system_competitor.txt ${team_cosmos}/config/system/system.txt"
    mv "${team_cosmos}/config/system/system_competitor.txt" "${team_cosmos}/config/system/system.txt"

    echo "sed -i \"s/@FLATSAT_IP@/${flatsat_sff_ip}/g\" \"${team_cosmos}/config/tools/cmd_tlm_server/cmd_tlm_server.txt\""
    sed -i "s/@FLATSAT_IP@/${flatsat_sff_ip}/g" "${team_cosmos}/config/tools/cmd_tlm_server/cmd_tlm_server.txt"

    echo "Copy targets from target list"
    echo "mkdir -p ${team_cosmos}/config/targets"
    for target in ${api_targets[@]}; do
        mkdir -p ${team_cosmos}/config/targets/${target}
        echo "cp -rv config/targets/${target}/ ${team_cosmos}/config/targets/"
        cp -rv "config/targets/${target}/" "${team_cosmos}/config/targets/"
    done

    cd ${team_dir}
    tar czf cosmos.tar.gz cosmos/
    cd ${COSMOS_PATH}

}


create_api_deployment() {

    cd ${COSMOS_PATH}
    team=$1
    output=$2
    flatsat_sff_ip=$3

    team_dir="${output}/team_${team}/api"
    team_cosmos="${team_dir}/cosmos"

    if [ -d ${team_cosmos} ]; then
        echo "Remove existing team dir ${team_cosmos} to ensure clean deployment"
        rm -rf ${team_cosmos}
    fi

    team_cosmos="${team_dir}/cosmos"

    echo "Create team output cosmos folder ${team_cosmos}"
    mkdir -p ${team_cosmos}

    echo "Copy Files for Deployment"

    echo "cp -rv cfs_kit/ ${team_cosmos}/"
    cp -rv cfs_kit/ ${team_cosmos}/

    echo "cp -rv outputs/ ${team_cosmos}/"
    cp -rv outputs/ ${team_cosmos}/

    echo "cp -rv tools/ ${team_cosmos}/"
    cp -rv tools/ ${team_cosmos}/

    echo "cp -rv scripts/ ${team_cosmos}/"
    cp -rv scripts/ ${team_cosmos}/

    echo "mkdir -p procedures/ ${team_cosmos}/"
    mkdir -p procedures/ ${team_cosmos}/
    
    echo "cp -rv lib/ ${team_cosmos}/"
    cp -rv lib/ ${team_cosmos}/

    nextcmd="rm ${team_cosmos}/lib/mqtt_job.rb"
    echo "$nextcmd"
    ${nextcmd}

    echo "Remove telemtry database library ruby file"
    echo "rm ${team_cosmos}/lib/telemetry_db.rb"
    rm ${team_cosmos}/lib/telemetry_db.rb

    echo "Copy files in cosmos folder"
    echo "find . -maxdepth 1 -type f -exec cp \"{}\" ${team_cosmos}/ \;"
    find . -maxdepth 1 -type f -exec cp "{}" ${team_cosmos}/ \;

    echo "Create output config folder"
    echo "mkdir -p ${team_cosmos}/config"
    mkdir -p ${team_cosmos}/config
    echo "cp -rv config/data ${team_cosmos}/config/"
    cp -rv config/data ${team_cosmos}/config/
    echo "cp -rv config/system ${team_cosmos}/config/"
    cp -rv config/system ${team_cosmos}/config/

    echo "cp -rv config/tools ${team_cosmos}/config/"
    cp -rv config/tools ${team_cosmos}/config/

    echo "Remove Config Files Not Needed for Team"
    echo "rm ${team_cosmos}/config/tools/launcher/launcher_monitor.txt"
    echo "rm ${team_cosmos}/config/tools/launcher/launcher_testbed.txt"
    echo "rm ${team_cosmos}/config/tools/launcher/launcher.txt"
    echo "rm ${team_cosmos}/config/tools/launcher/launcher_competitor.txt"
    rm "${team_cosmos}/config/tools/launcher/launcher_monitor.txt"
    rm "${team_cosmos}/config/tools/launcher/launcher_testbed.txt"
    rm "${team_cosmos}/config/tools/launcher/launcher.txt"
    rm "${team_cosmos}/config/tools/launcher/launcher_competitor.txt"

    echo "rm ${team_cosmos}/config/tools/cmd_tlm_server/cmd_tlm_server_digitaltwin_testbed.txt"
    echo "rm ${team_cosmos}/config/tools/cmd_tlm_server/cmd_tlm_server_digitaltwin.txt"
    echo "rm ${team_cosmos}/config/tools/cmd_tlm_server/cmd_tlm_server_pdb.txt"
    echo "rm ${team_cosmos}/config/tools/cmd_tlm_server/cmd_tlm_server.txt"
    echo "rm ${team_cosmos}/config/tools/cmd_tlm_server/cmd_tlm_server_competitor.txt"
    rm "${team_cosmos}/config/tools/cmd_tlm_server/cmd_tlm_server_digitaltwin_testbed.txt"
    rm "${team_cosmos}/config/tools/cmd_tlm_server/cmd_tlm_server_challenge.txt"
    rm "${team_cosmos}/config/tools/cmd_tlm_server/cmd_tlm_server_digitaltwin.txt"
    rm "${team_cosmos}/config/tools/cmd_tlm_server/cmd_tlm_server_pdb.txt"
    rm "${team_cosmos}/config/tools/cmd_tlm_server/cmd_tlm_server.txt"
    rm "${team_cosmos}/config/tools/cmd_tlm_server/cmd_tlm_server_competitor.txt"
    echo "rm ${team_cosmos}/config/system/system.txt"
    rm ${team_cosmos}/config/system/system.txt
    echo "rm ${team_cosmos}/config/system/system_competitor.txt"
    rm ${team_cosmos}/config/system/system_competitor.txt
    echo "rm ${team_cosmos}/run.sh"
    rm ${team_cosmos}/run.sh
    echo "rm ${team_cosmos}/start.sh"
    rm ${team_cosmos}/start.sh

    echo "Rename files for competitor distribution"
    echo "mv ${team_cosmos}/config/tools/cmd_tlm_server/cmd_tlm_server_api.txt ${team_cosmos}/config/tools/cmd_tlm_server/cmd_tlm_server_api.txt"
    mv "${team_cosmos}/config/tools/cmd_tlm_server/cmd_tlm_server_api.txt" "${team_cosmos}/config/tools/cmd_tlm_server/cmd_tlm_server.txt"
    echo "mv ${team_cosmos}/config/tools/launcher/launcher_api.txt ${team_cosmos}/config/tools/launcher/launcher.txt"
    mv "${team_cosmos}/config/tools/launcher/launcher_api.txt" "${team_cosmos}/config/tools/launcher/launcher.txt"
    echo "mv ${team_cosmos}/config/system/system_api.txt ${team_cosmos}/config/system/system.txt"
    mv "${team_cosmos}/config/system/system_api.txt" "${team_cosmos}/config/system/system.txt"
    echo "mv ${team_cosmos}/run_api.sh ${team_cosmos}/run.sh"
    mv ${team_cosmos}/run_api.sh ${team_cosmos}/run.sh
    echo "mv ${team_cosmos}/start_api.sh ${team_cosmos}/run.sh"
    mv ${team_cosmos}/start_api.sh ${team_cosmos}/start.sh

    echo "sed -i \"s/@FLATSAT_IP@/${flatsat_sff_ip}/g\" \"${team_cosmos}/config/tools/cmd_tlm_server/cmd_tlm_server.txt\""
    sed -i "s/@FLATSAT_IP@/${flatsat_sff_ip}/g" "${team_cosmos}/config/tools/cmd_tlm_server/cmd_tlm_server.txt"

    echo "Copy targets from target list"
    echo "mkdir -p ${team_cosmos}/config/targets"
    for target in ${api_targets[@]}; do
        mkdir -p ${team_cosmos}/config/targets/${target}
        echo "cp -rv config/targets/${target}/ ${team_cosmos}/config/targets/"
        cp -rv "config/targets/${target}/" "${team_cosmos}/config/targets/"
    done

    cd ${team_dir}
    tar czf cosmos.tar.gz cosmos/
    cd ${COSMOS_PATH}

}


create_api_challenge_deployment() {

    cd ${COSMOS_PATH}
    team=$1
    output=$2
    flatsat_sff_ip=$3

    team_dir="${output}/team_${team}/challenge"
    team_cosmos="${team_dir}/cosmos"

    if [ -d ${team_cosmos} ]; then
        echo "Remove existing team dir ${team_cosmos} to ensure clean deployment"
        rm -rf ${team_cosmos}
    fi

    team_cosmos="${team_dir}/cosmos"

    echo "Create team output cosmos folder ${team_cosmos}"
    mkdir -p ${team_cosmos}

    echo "Copy Files for Deployment"

    echo "cp -rv cfs_kit/ ${team_cosmos}/"
    cp -rv cfs_kit/ ${team_cosmos}/

    echo "cp -rv outputs/ ${team_cosmos}/"
    cp -rv outputs/ ${team_cosmos}/

    echo "cp -rv tools/ ${team_cosmos}/"
    cp -rv tools/ ${team_cosmos}/

    echo "cp -rv scripts/ ${team_cosmos}/"
    cp -rv scripts/ ${team_cosmos}/

    echo "mkdir -p procedures/ ${team_cosmos}/"
    mkdir -p procedures/ ${team_cosmos}/

    echo "cp -rv lib/ ${team_cosmos}/"
    cp -rv lib/ ${team_cosmos}/

    nextcmd="rm ${team_cosmos}/lib/mqtt_job.rb"
    echo "$nextcmd"
    ${nextcmd}

    echo "Remove telemtry database library ruby file"
    echo "rm ${team_cosmos}/lib/telemetry_db.rb"
    rm ${team_cosmos}/lib/telemetry_db.rb

    echo "Copy files in cosmos folder"
    echo "find . -maxdepth 1 -type f -exec cp \"{}\" ${team_cosmos}/ \;"
    find . -maxdepth 1 -type f -exec cp "{}" ${team_cosmos}/ \;

    echo "Create output config folder"
    echo "mkdir -p ${team_cosmos}/config"
    mkdir -p ${team_cosmos}/config
    echo "cp -rv config/data ${team_cosmos}/config/"
    cp -rv config/data ${team_cosmos}/config/
    echo "cp -rv config/system ${team_cosmos}/config/"
    cp -rv config/system ${team_cosmos}/config/

    echo "cp -rv config/tools ${team_cosmos}/config/"
    cp -rv config/tools ${team_cosmos}/config/

    echo "Remove Config Files Not Needed for Team"
    echo "rm ${team_cosmos}/config/tools/launcher/launcher_monitor.create_challenger_deploymenttxt"
    echo "rm ${team_cosmos}/config/tools/launcher/launcher_testbed.txt"
    echo "rm ${team_cosmos}/config/tools/launcher/launcher.txt"
    echo "rm ${team_cosmos}/config/tools/launcher/launcher_competitor.txt"
    rm "${team_cosmos}/config/tools/launcher/launcher_monitor.txt"
    rm "${team_cosmos}/config/tools/launcher/launcher_testbed.txt"
    rm "${team_cosmos}/config/tools/launcher/launcher.txt"
    rm "${team_cosmos}/config/tools/launcher/launcher_competitor.txt"

    echo "rm ${team_cosmos}/config/tools/cmd_tlm_server/cmd_tlm_server_digitaltwin_testbed.txt"
    echo "rm ${team_cosmos}/config/tools/cmd_tlm_server/cmd_tlm_server_digitaltwin.txt"
    echo "rm ${team_cosmos}/config/tools/cmd_tlm_server/cmd_tlm_server_pdb.txt"
    echo "rm ${team_cosmos}/config/tools/cmd_tlm_server/cmd_tlm_server.txt"
    echo "rm ${team_cosmos}/config/tools/cmd_tlm_server/cmd_tlm_server_competitor.txt"
    rm "${team_cosmos}/config/tools/cmd_tlm_server/cmd_tlm_server_digitaltwin_testbed.txt"
    rm "${team_cosmos}/config/tools/cmd_tlm_server/cmd_tlm_server_digitaltwin.txt"
    rm "${team_cosmos}/config/tools/cmd_tlm_server/cmd_tlm_server_pdb.txt"
    rm "${team_cosmos}/config/tools/cmd_tlm_server/cmd_tlm_server.txt"
    rm "${team_cosmos}/config/tools/cmd_tlm_server/cmd_tlm_server_api.txt"
    rm "${team_cosmos}/config/tools/cmd_tlm_server/cmd_tlm_server_competitor.txt"
    echo "rm ${team_cosmos}/config/system/system.txt"
    rm ${team_cosmos}/config/system/system.txt
    echo "rm ${team_cosmos}/config/system/system_api.txt"
    rm ${team_cosmos}/config/system/system_api.txt
    echo "rm ${team_cosmos}/config/system/system_competitor.txt"
    rm ${team_cosmos}/config/system/system_competitor.txt
    echo "rm ${team_cosmos}/run.sh"
    rm ${team_cosmos}/run.sh
    echo "rm ${team_cosmos}/start.sh"
    rm ${team_cosmos}/start.sh

    echo "Rename files for competitor distribution"
    echo "mv ${team_cosmos}/config/tools/cmd_tlm_server/cmd_tlm_server_challenge.txt ${team_cosmos}/config/tools/cmd_tlm_server/cmd_tlm_server.txt"
    mv "${team_cosmos}/config/tools/cmd_tlm_server/cmd_tlm_server_challenge.txt" "${team_cosmos}/config/tools/cmd_tlm_server/cmd_tlm_server.txt"
    echo "mv ${team_cosmos}/config/tools/launcher/launcher_api.txt ${team_cosmos}/config/tools/launcher/launcher.txt"
    mv "${team_cosmos}/config/tools/launcher/launcher_api.txt" "${team_cosmos}/config/tools/launcher/launcher.txt"
    echo "mv ${team_cosmos}/config/system/system_api.txt ${team_cosmos}/config/system/system.txt"
    mv "${team_cosmos}/config/system/system_challenge.txt" "${team_cosmos}/config/system/system.txt"
    echo "mv ${team_cosmos}/run_api.sh ${team_cosmos}/run.sh"
    mv ${team_cosmos}/run_api.sh ${team_cosmos}/run.sh
    echo "mv ${team_cosmos}/start_api.sh ${team_cosmos}/run.sh"
    mv ${team_cosmos}/start_api.sh ${team_cosmos}/start.sh

    echo "sed -i \"s/@FLATSAT_IP@/${flatsat_sff_ip}/g\" \"${team_cosmos}/config/tools/cmd_tlm_server/cmd_tlm_server.txt\""
    sed -i "s/@FLATSAT_IP@/${flatsat_sff_ip}/g" "${team_cosmos}/config/tools/cmd_tlm_server/cmd_tlm_server.txt"

    echo "Copy targets from target list"
    echo "mkdir -p ${team_cosmos}/config/targets"
    for target in ${api_targets[@]}; do
        mkdir -p ${team_cosmos}/config/targets/${target}
        echo "cp -rv config/targets/${target}/ ${team_cosmos}/config/targets/"
        cp -rv "config/targets/${target}/" "${team_cosmos}/config/targets/"
    done

    cd ${team_dir}
    tar czf cosmos.tar.gz cosmos/
    cd ${COSMOS_PATH}

}

echo "Create Cosmos Deployment"
CUR_DIR=$(pwd)
COSMOS_PATH="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/cosmos"
team_defined=0
team_num=1
output_defined=0
output_dir="${CUR_DIR}/cosmos_deployment"
compresss_output=0
git_clean=0
use_test_network=${USE_TEST_NETWORK:-0}

teams_all=(1 2 3 4 5 6 7 8 9 10)
flatsat_sff_ips=("192.168.200.11" "192.168.200.12" "192.168.200.13" "192.168.200.14" "192.168.200.15" "192.168.200.16" "192.168.200.17" "192.168.200.18" "192.168.200.19" "192.168.200.20")

usage="Usage: create_deployment.sh -t <team_num> -o <output_folder> -c (compress output folder) Create team cosmos deployment"

while getopts "h:t:o:cg" option; 
do
    case "$option" in
        h|\?)
            echo ${usage};
            exit;;
        t )
            team_num=$OPTARG
            team_defined=1
            ;;
        o )
            output_dir=$OPTARG
            output_defined=1
            ;;
        c ) 
            compresss_output=1
            ;;
        g )
            git_clean=1
            ;;
        : )
            echo "Missing option argument for -$OPTARG" >&2; exit 1;;
        *  )
            echo "Unimplemented option: -$OPTARG" >&2; exit 1;;
    esac
done

output_dir=$(realpath ${output_dir})
team_index=$(($team_num - 1))

if [ ${git_clean} -eq 1 ]; then
    echo "Remove file from cosmos folder not tracked by git"
    echo "Files to be removed"
    git clean -xdn
    git clean -xdf
fi

if [ ${team_defined} -eq 1 ]; then
    echo "Create Team ${team_num} Cosmos Deployment in Directory: ${output_dir}"
    create_challenger_deployment ${team_num} ${output_dir} ${flatsat_sff_ips[${team_index}]}
    #create_api_deployment ${team_num} ${output_dir} ${flatsat_sff_ips[${team_index}]}
    #create_api_challenge_deployment ${team_num} ${output_dir} ${flatsat_sff_ips[${team_index}]}
    echo "Finished team ${team_num}, team_index ${team_index}"
else
    echo "Create Team ${team_num} Cosmos Deployment in Directory: ${output_dir}"
    for team in ${teams_all[@]}; do
        create_challenger_deployment ${team} ${output_dir} ${flatsat_sff_ips[${team_index}]}
        create_api_deployment ${team_num} ${output_dir} ${flatsat_sff_ips[${team_index}]}
        create_api_challenge_deployment ${team_num} ${output_dir} ${flatsat_sff_ips[${team_index}]}
    done
fi

if [ ${compresss_output} -eq 1 ]; then
    if [ -d ${output_dir} ]; then
        echo "Create compressed output archive"
        archive_name=$(basename ${output_dir}).tar.gz
        echo "Archive name: ${archive_name}"
        echo "tar czf ${CUR_DIR}/${archive_name} -C ${output_dir}/ ."
        tar czf ${CUR_DIR}/${archive_name} -C "${output_dir}/" .
    fi
fi
