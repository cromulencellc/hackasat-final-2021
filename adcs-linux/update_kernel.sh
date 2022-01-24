#!/bin/bash

echo "Running Petalinux Config to Update Kernel Configuration "

source setup_env.sh

echo "Run kernel configuration"
petalinux-config -c kernel

echo "Commit changes"

petalinux-devtool finish linux-xlnx $(pwd)/project-spec/meta-user/
echo "Complete"