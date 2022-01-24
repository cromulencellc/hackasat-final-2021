#!/bin/bash

IMAGE_FILE=$1
VMDK_FILE=$2

if [ -z "${IMAGE_FILE}" ]; then
    echo "No image file provided."
    exit 1
fi

if [ ! -f "${IMAGE_FILE}" ]; then
echo "Image File: '${IMAGE_FILE}' not a file."
exit 1
fi


if qemu-img convert -f raw -O vmdk ${IMAGE_FILE} ${VMDK_FILE}; then
    echo "VMDK created successfully"
else 
    echo "VMDK creation failed"
    exit 1
fi

sudo chown $USER:$USER ${VMDK_FILE}