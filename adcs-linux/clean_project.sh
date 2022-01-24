#!/bin/bash

echo "Cleaning petalinux workspace"

source setup_env.sh

petalinux-build -x mrproper -f
