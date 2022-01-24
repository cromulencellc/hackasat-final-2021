#!/bin/bash

echo "Running Petalinux Build"

source setup_env.sh

if [ $? -ne 0 ]; then
    echo "petalinux-config did not finish with exit status 0, exiting..."
    exit 1
fi

echo "Running petalinux build"
petalinux-build


if [ $? -ne 0 ]; then
    echo "petalinux-build did not finish with exit status 0, exiting..."
    exit 1
fi

HW_DIR="/home/${USER}/adcs-zynq/hw"

if [ ! -f ${HW_BIT} ]; then
    echo "HW Bitstream File not Found in ${HW_DIR}, exiting..."
    exit 1
fi

echo "Running petalinux package"
petalinux-package --boot --fsbl images/linux/zynq_fsbl.elf --fpga images/linux/system.bit --u-boot --force

echo "Create plain text device tree file"
dtc -I dtb -O dts images/linux/system.dtb -o images/linux/system.dts
echo "Complete!"