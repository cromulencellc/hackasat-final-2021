#!/bin/bash

## God Bless America, Madonna, and the ADCS

MAX_CRASH_COUNT=5000

crash_count=0
while [ $crash_count -le $MAX_CRASH_COUNT ]
do
    cd /apps/cpu1/
    echo -e "\n\nStarting ADCS flight software\n\n"
    ./core-cpu1
    crash_count=$(($crash_count + 1))
    echo -e "ADCS crashed with return code: $?"
    echo -e "This is crash # $crash_count"
    echo -e "Killing residual core-cpu1"
    pgrep core-cpu1 | xargs kill -9
done
