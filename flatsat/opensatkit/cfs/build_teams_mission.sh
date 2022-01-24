#!/bin/bash

mkdir -p build_teams

function build_adcs(){
    #### ADCS ####
    export TEAM_NUM=$1

    rm -rf build_teams/team_$TEAM_NUM/adcs/
    mkdir -p build_teams/team_$TEAM_NUM/adcs/
    mkdir -p build_teams_all/team_$TEAM_NUM/adcs/

    echo -e "Building team $TEAM_NUM adcs binaries\n"
    mkdir -p build_teams/team_$TEAM_NUM/adcs/
    ./build_mission.sh osk_adcs 0 1     # <mission_name> <incremental_build> <challenge_build>
    if [ $? -ne 0 ]; then
        exit 1
    fi

    cp -r build_osk_adcs/exe/cpu1 build_teams/team_$TEAM_NUM/adcs/
    # cp -r build_osk_adcs build_teams_all/team_$TEAM_NUM/adcs

    echo -e "Finished building team $TEAM_NUM adcs binaries (build_teams/team_$TEAM_NUM/adcs)\n"
}

function build_cdh(){
    export TEAM_NUM=$1

    ##### CDH #####
    echo -e "Building team $TEAM_NUM cdh binaries\n"
    rm -rf build_teams/team_$TEAM_NUM/cdh/
    mkdir -p build_teams/team_$TEAM_NUM/cdh/
    ./build_mission.sh osk_cdh 0 1      # <mission_name> <incremental_build> <challenge_build>
    if [ $? -ne 0 ]; then
        exit 1
    fi
    cp build_osk_cdh/exe/cpu1/core-cpu1.exe build_teams/team_$TEAM_NUM/cdh/
    echo -e "Finished building team $TEAM_NUM cdh binaries (build_teams/team_$TEAM_NUM/cdh)\n"
}

if [ -z $1 ]; then
    echo "Mission not set, exiting..."
    exit 1
fi

for team_num in {1..8}
    do
        echo -e "Building team $team_num binaries\n"
        if [ "$1" == "osk_cdh" ]; then
            build_cdh "$team_num"
        elif [ "$1" == "osk_adcs" ]; then
            build_adcs "$team_num"
        else
            echo "$1"
        fi
    done

