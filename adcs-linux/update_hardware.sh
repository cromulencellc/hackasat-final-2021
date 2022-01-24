#!/bin/bash

echo "Running Petalinux Config to Update HW Configuration "

source setup_env.sh

curDir=`pwd`
echo "Current Directory: ${curDir}"
HW_DIR="/home/${USER}/adcs-zynq/hw"
HW_XSA=${HW_DIR}/adcs_bd_wrapper.xsa


if [ ! -f ${HW_XSA} ]; then
    echo "HW Description ZIP File not Found in ${HW_DIR}, exiting..."
    exit 1
fi

echo "Getting HW Config and Loading in Petalinux tools"
petalinux-config --get-hw-description ${HW_DIR}

echo "Complete"
