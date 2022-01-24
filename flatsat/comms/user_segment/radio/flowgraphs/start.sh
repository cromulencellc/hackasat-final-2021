#!/usr/bin/env bash


echo "Compile shared flow graphs"

grcc ../../../shared/flowgraphs/dependencies/*.grc

echo "Compile usersegment_wideband_tx"

grcc usersegment_wideband_tx.grc

if [ ! -f "$HOME/.volk/volk_config" ]; then
    echo "Running volk_profile, this takes a bit, but only needs to happen once"
    volk_profile
else 
    echo "volk_profile exists already exists. Excellent!"
fi

echo "Starting Flowgraph"
./usersegment_wideband_tx.py
