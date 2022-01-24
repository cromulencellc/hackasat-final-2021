#!/bin/bash


IMAGE_FILE=$1
BOOT_LABEL=$2
ROOT_LABEL=$3

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
done < <(sudo kpartx -l "${IMAGE_FILE}")))

BOOT_DEV=${KPARTX_DEVS[0]}
ROOT_DEV=${KPARTX_DEVS[1]}

sudo fatlabel ${BOOT_DEV} ${BOOT_LABEL}
sudo e2label ${ROOT_DEV} ${ROOT_LABEL}