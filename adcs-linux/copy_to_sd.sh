#!/bin/bash

echo "Check for micro SD card mounted to system"
BOOT_MOUNT=/media/${USER}/BOOT
ROOTFS_MOUNT=/media/${USER}/rootfs

if [ ! -d ${BOOT_MOUNT} ]; then
    echo "SD boot partition not found, exiting"
    exit 1
fi

if [ ! -d ${ROOTFS_MOUNT} ]; then
    echo "SD rootfs partition not found, exiting"
    exit 1
fi

echo "Prepare Kernel Modules for transfer"
ROOTFS_BUILD_TAR=images/linux/rootfs.tar.gz
MODULES_STAGING_DIR=images/linux/tmp
MODULES_DIR=images/linux/tmp/lib/modules

mkdir -p ${MODULES_STAGING_DIR}
tar -xf ${ROOTFS_BUILD_TAR} -C ${MODULES_STAGING_DIR}

echo "Copy Kernel Modules to rootfs"
sudo cp -r ${MODULES_DIR}/* ${ROOTFS_MOUNT}/lib/modules

echo "Copy bootloader files to micro SD card"
echo "Check for bootloader build artifacts"
BOOT_BIN=images/linux/BOOT.BIN
BOOT_SCR=images/linux/boot.scr
BOOT_DTB=images/linux/system.dtb
BOOT_IMAGE=images/linux/uImage

# For 2020.1 Need additional boot.scr file
files=(${BOOT_BIN} ${BOOT_SCR} ${BOOT_IMAGE})
#files=(${BOOT_BIN} ${BOOT_IMAGE} ${BOOT_DTB})


for i in ${files[@]}; do
    if [ ! -f ${i} ]; then
        echo "Build artifact ${i} not found, exiting..."
        exit 1
    fi
    echo "cp ${i} /media/${USER}/boot"
    cp ${i} ${BOOT_MOUNT}/
done
sleep 2
sync
sleep 2

DISK_1=`df -h | grep ${BOOT_MOUNT} | awk -F" " '{ print $1 }'`
DISK_2=`df -h | grep ${ROOTFS_MOUNT} | awk -F" " '{ print $1 }'`
DISK=`echo ${DISK_1} | sed 's/[0-9]*//g'`
echo "Unmount ${DISK_1}"
udisksctl unmount -b ${DISK_1}
echo "Unmount ${DISK_2}"
udisksctl unmount -b ${DISK_2}
echo "Power-off ${DISK}"
udisksctl power-off -b ${DISK}

echo "Complete"





