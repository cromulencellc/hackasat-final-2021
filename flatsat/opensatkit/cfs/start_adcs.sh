#!/bin/bash

cfs_dir=build_osk_adcs/exe/cpu1

if [ -d ${cfs_dir} ]; then

    cd ${cfs_dir}
    
    if [ -f core-cpu1 ]; then
        ./core-cpu1
    fi

fi