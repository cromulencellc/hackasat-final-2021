#!/usr/bin/bash 

GRMON_BAUD=${GRMON_BAUD:-921600}
GRMON_EDCL_IP="192.168.0.51"

TOOLS_DOCKER_IMAGE="has2/finals/cdh_board_tools:latest"
ABSOLUTE_PATH="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
export PATH=${PATH}:${ABSOLUTE_PATH}/grmon/linux/bin64
export PATH=${PATH}:${ABSOLUTE_PATH}/ztex/java/FWLoader

IFS='' read -r -d '' GRMON_USAGE <<'EOF'
C&DH grmon tool macros:
Usage: cdh /dev/ttyUSB<#> <command>
Commands:
  shell: interactive terminal, allows for flashing/running  
  debug: starts the gdb remote console
  attach: attach to grmon gdb remote server
  program: attach to grmon and program flash. grmon /dev/ttyUSB<#> program <prom_file>
  programv: attach to grmon and program flash and verify. grmon /dev/ttyUSB<#> program <prom_file>
  eshell: interactive terminal, allows for flashing/running. Uses Ethernet EDCL link. grmon edebug <IP (Default: 192.168.0.51)>
  edebug: starts the gdb remote console. Uses Ethernet EDCL link. grmon edebug <IP (Default: 192.168.0.51)>
EOF

IFS='' read -r -d '' ZTEX_USAGE <<'EOF'
ztex tool macros:
Usage: ztex <command>
Commands:
  ping:     Check ztex is available
  program:  Write bit stream to the FPGA flash. ztex program <bitstream_file>
Note: Runs ztex FPGA commands on localhost (not Docker)
EOF

IFS='' read -r -d '' MKPROM2_USAGE <<'EOF'
mkprom2 tool macros:
Usage: mkprom2 <input_name.exe> <output_name.prom>
Note: The current working directory is mapped in
      relative paths work, but not up from the current location
EOF

tool_usage() {
  echo "Board Tools Usage:"
  # echo "${TOOL_SHELL_USAGE}"
  echo "${GRMON_USAGE}"
  echo "${ZTEX_USAGE}"
  echo "${MKPROM2_USAGE}"
}


cdh() {
  if [ "$#" -ge 2 ]; then 
    if [ "$1" = "shell" ]; then
      # docker run --rm -it --name=grmon_cli --privileged --device=$2 -v $PWD:/mnt ${TOOLS_DOCKER_IMAGE} "grmon -uart $2 -baud ${GRMON_BAUD}"
     grmon -rtems rtems-5.0 -uart $2 -baud ${GRMON_BAUD}
    elif [ "$1" = "debug" ]; then
      # docker run --rm -it --name=grmon_gdb_remote --net=host --privileged --device=$2 -v $PWD:/mnt ${TOOLS_DOCKER_IMAGE} "grmon -uart $2 -baud ${GRMON_BAUD} -gdb"
      grmon -rtems rtems-5.0 -uart $2 -baud ${GRMON_BAUD} -gdb
    elif [ "$1" = "eshell" ]; then
      # docker run --rm -it --name=grmon_cli --net=host -v $PWD:/mnt ${TOOLS_DOCKER_IMAGE} "grmon -eth $2"
      grmon -rtems rtems-5.0 -eth $2
    elif [ "$1" = "eping" ]; then
      grmon -eth $2 -e "quit"
    elif [ "$1" = "edebug" ]; then
      # docker run --rm -it --name=grmon_gdb_remote --net=host -v $PWD:/mnt ${TOOLS_DOCKER_IMAGE} "grmon -eth $2 -gdb"
      grmon -rtems rtems-5.0 -eth $2 -gdb

    elif [ "$1" = "edcl-set-ip" ]; then
      # docker run --rm -it --name=grmon_gdb_remote --net=host -v $PWD:/mnt ${TOOLS_DOCKER_IMAGE} "grmon -eth $2 -gdb"
      cat << EOF > grmon.script
edcl $3 greth0
quit
EOF
      grmon -rtems rtems-5.0 -eth $2 -c grmon.script

    elif [ "$1" = "attach" ]; then
      IP=$(docker inspect -f '{{range .NetworkSettings.Networks}}{{.IPAddress}}{{end}}' grmon_gdb_remote)
      echo "Loading will take some time..."
      echo "-------------------------------"
      docker run --rm -it --name=grmon_gdb_attach --net="host" -v ${PWD}:/mnt ${TOOLS_DOCKER_IMAGE} "sparc-rtems5-gdb $2 -ex 'target extended-remote localhost:2222' -ex 'directory /mnt/' -ex 'load'"

    elif [ "$1" = "program" ]; then
      echo "Programming Leon 3"
      cat << EOF > grmon.script
spim flash detect
spim flash erase
spim flash load $3
EOF
      grmon -rtems rtems-5.0 -uart $2 -baud ${GRMON_BAUD} -c grmon.script
      rm grmon.script
    elif [ "$1" = "programv" ]; then
      echo "Programming Leon 3 with Flash Verification"
      cat << EOF > grmonv.script
spim flash detect
spim flash erase
spim flash load $3
spim flash verify $3
EOF
      grmon -rtems rtems-5.0 -uart $2 -baud ${GRMON_BAUD} -c grmon.script
      rm grmonv.script
    else
      echo "Unrecognized command"
      echo "----------------------"
      echo "${GRMON_USAGE}"
    fi
  
  else
    echo "${GRMON_USAGE}"
  fi
}

ztex() {
  
  if [ "$#" -ge 1 ]; then
    if [ "$1" = "program" ]; then
      if [ -f "${2}" ]; then
        echo "Program FPGA Bitstream: $2"
        echo "FWLoader -c -um $2"
        FWLoader -c -um $2
      else 
        echo "Bitstream file ${2} doesn't exist"
      fi
    elif [ "$1" = "ping" ]; then
      echo "Check for connected ZTEX FGPAs on USB"
      FWLoader -c -i
    fi
  else
    echo "ZTEX Incorrect Arguments"
    echo "${ZTEX_USAGE}"
  fi
}

mkprom2() { 
  if [ $# -ne 2 ]; then
    echo "${MKPROM2_USAGE}"
  else
    sudo ln -sf /opt/mkprom2 ${ABSOLUTE_PATH}}/mkprom2
    export PATH=${PATH}:/opt/mkprom2
      # Ramsize should be 256MB, let's do 128MB for safety (because it works)
    mkprom2 -leon3 -msoft-float -freq 99 -baud 38431 -ramsize 131072 $1 -o $2

  fi
}

docker_build() {
  docker build -t ${TOOLS_DOCKER_IMAGE} .
  docker tag ${TOOLS_DOCKER_IMAGE} "has2/finals/cdh_board_tools:latest"
}

list_ports() {
  python3 -m serial.tools.list_ports -v
}