#!/bin/sh

# Startup COMM payload services
echo "Starting COMM payload"

if [ -f /opt/payload/flowgraphs/flatsat_transceiver_headless.py ]; then
    /usr/bin/python3 /opt/payload/flowgraphs/flatsat_transceiver_headless.py
fi

# Exit
exit 0
