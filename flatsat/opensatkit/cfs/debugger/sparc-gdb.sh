#!/bin/bash


curDir=$(pwd)
oskDir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && cd .. && pwd)"
scriptDir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

exefile="build_osk_cdh/exe/cpu1/core-cpu1.exe"
containerBaseDir="/apps/opensatkit/cfs/"
if [ "$#" -eq 1 ]; then
    exefile=$1
fi

echo "Start GDB with ${oskDir}/${exefile}"
if [ -f "${oskDir}/${exefile}" ]; then
    # cp "${exefile}" "${scriptDir}"
    cd ${oskDir}
    echo "$(pwd)"
    echo "$(ls)" 
    docker run \
        -it \
        --rm \
        --net=host \
        -v ${oskDir}:/apps/opensatkit/cfs \
        --user $(id -u):$(id -g) \
        --entrypoint="/opt/rtems/5/bin/sparc-rtems5-gdb" \
        has2/finals/opensatkit-docker/cfs-builder/rtems:latest \
        "-x" "debugger/cdh-restart.py" "-x" "debugger/sparc.gdbinit" "${containerBaseDir}/${exefile}"
    retStatus=0
else
    echo "No executable found. Please try rebuilding software"
    retStatus=1
fi
cd ${curDir}
exit(retStatus)
