#!/bin/bash


IMAGE_FILE=$1

if [ -z "${IMAGE_FILE}" ]; then
    echo "No image file provided."
    exit 1
fi

if [ ! -f "${IMAGE_FILE}" ]; then
echo "Image File: '${IMAGE_FILE}' not a file."
exit 1
fi

# Figure out the device names that kpartx will create
declare -a KPARTX_DEVS
KPARTX_DEVS=($(while read dev rest; do
echo "/dev/mapper/${dev}"
done < <(sudo kpartx -v -l "${IMAGE_FILE}")))

BOOT_DEV=${KPARTX_DEVS[0]}
ROOT_DEV=${KPARTX_DEVS[1]}

BOOT_MOUNTPOINT=img/boot
ROOT_MOUNTPOINT=img/root

echo "  BOOT_DEV: ${BOOT_DEV}"
echo "  ROOT_DEV: ${ROOT_DEV}"

sudo kpartx -v -a "${IMAGE_FILE}"

mkdir -p ${BOOT_MOUNTPOINT}
mkdir -p ${ROOT_MOUNTPOINT}

if sudo mount ${ROOT_DEV} ${ROOT_MOUNTPOINT}; then
    echo "Root partition mounted successfully"
  else
    echo "Failed to mount root partition"
    exit 1
fi 

if sudo mount ${BOOT_DEV} ${BOOT_MOUNTPOINT}; then
    echo "Boot partition mounted successfully"
  else
    echo "Failed to mount boot partition"
    exit 1
fi


