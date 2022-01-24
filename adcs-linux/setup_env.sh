#!/bin/bash
echo "Setup Petalinux Toolchain"

PETALINUX_ENV_HOME="${PETALINUX_HOME:-/home/${USER}/petalinux}"
export PETALINUX_HOME=${PETALINUX_ENV_HOME}
source ${PETALINUX_ENV_HOME}/settings.sh
export MAKEFLAGS=-j8

