#!/bin/sh

# Fix Ethernet Speed to 100Mbps
/sbin/ethtool -s eth0 speed 100 duplex full

# Exit
exit 0
