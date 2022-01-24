#!/bin/bash

ssh adcs@$ADCS_IP "rm -rf /apps/cpu1" &
./build.sh -m osk_adcs -t ${TEAM_NUMBER}
sleep 2
scp -r build_osk_adcs/exe/cpu1 adcs@$ADCS_IP:/apps/
