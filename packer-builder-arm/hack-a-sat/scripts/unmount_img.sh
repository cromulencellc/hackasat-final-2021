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

BOOT_MOUNTPOINT=img/boot
ROOT_MOUNTPOINT=img/root

if sudo umount ${BOOT_MOUNTPOINT}; then
    echo "Boot partition unmounted successfully"
else
    echo "Failed to unmount boot partition"
    exit 1
fi

if sudo umount ${ROOT_MOUNTPOINT}; then
    echo "Root partition unmounted successfully"
else
    echo "Failed to unmount root partition"
    exit 1
fi

if sudo kpartx -v -d "${IMAGE_FILE}"; then
    echo "Loop mounts removed successfully"
else
    echo "Failed to remove loop mounts"
    exit 1
fi

rm -rf img/