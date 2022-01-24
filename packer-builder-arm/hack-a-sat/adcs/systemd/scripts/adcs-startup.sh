#!/bin/sh

# Startup ADCS FSW On Boot
echo "Starting ADCS FSW"

if [ -f /apps/cpu1/core-cpu1 ] && [ -d /apps/cpu1/cf ]; then
    cd /apps/cpu1 && ./core-cpu1
fi

# Exit
exit 0
