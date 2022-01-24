<p align="center">
<img src="documentation/images/HAS_logo.png" alt=has2_logo width=400>
</p>

---

# Hack-a-Sat 2 FlatSat User Guide



Unified repository for FlatSat hardware and software deployment for development and runtime. This document is a Users-Guide for setup and running software on the FlatSat. 

## Table of Contents

* [Install flatsat tools](#flatsat-tools-install)
* [Executing flatsat tools](#executing-flatsat-tools)
* [FlatSat Setup](#flasat-setup)

## FlatSat Tools Install

```shell
./install.sh
```

The above script should install Docker, but the instructions below are linked as a backup.

- Requires Docker to be installed and configured on host
  - [Install Docker Official Instructions](https://docs.docker.com/engine/install/ubuntu/)
  - Non root docker setup [Setup non root docker](https://docs.docker.com/engine/install/linux-postinstall/)

## Executing Flatsat Tools
This section describes how to run the flatsat tools nominally. Assumes flatsat is fully programed, all SD cards are flashed, all inter-board wiring is connected, and you want do run the flatsat for development.
### Tool Statup
1. Choose team number. Usually this is 1 for developement
2. Get network interface for Ethernet adaptor
3. Connect desired FlatSat via ethernet to host computer
4. Connect CDH USB-C for CFS Debug output (for debug aid) to left side USB-C port (toward 3 picoblade connectors)
```shell
nmcli con show
```
Probably will be enx* something
3. Run startup script
```shell
./flatsat.sh -t <team_num> -i <ethernet_inferface> -b <optional_flag_to_build_cfs>
```

### Manual Shutdown
Usually handeled automatically when you exit out of tools, but manual instructions below.
```shell
./stop.sh -t <team_number>
```
## FlatSat Setup

### GSE Requirements
1. Working FlatSat electronics stack
    - See FlatSat build guide [TBR]
2. USB-C Cables to connect to C&DH (Console & Debug)
3. USB Micro cable to connect to PDB
4. USB Mini cable to program C&DH Ztex FPGA
5. Ethernet to Picoblade
6. Ethenet cable to computer (Good luck so far with USB to Ethernet adapters)
7. Remove before flight audio connector
8. USB-PD Power Supply (>45W)
9. USB-C to USB-C Cable for PD power


### Development Credentials
#### Linux Credentials
|Username|Password|Notes|
|---|---|---|
|adcs|adcs|Non-priviledged user used to run CFS software|
|adcsadm|adcsadm|Priviledged user with sudo for managment|
|comm|comm|Non-priviledged user used to run GNU radio and game software|
|commadm|commadm|Priviledged user with sudo for managment|

#### MQTT Credentials
|Username|Password|Role|Notes|
|---|---|---|---|
|comm|2B5458962584FE417B2803CF|game_with_sim_read|Comm MQTT User/Pass|
|cfs_adcs|88917673D3F3EE01E715F681|game_with_sim|MQTT ADCS CFS User/Pass|
|cfs_cdh|07CB501D6555EAC685EB6DCC|game|MQTT ADCS CFS User/Pass|
|42|F99BD16958F2ACB72731BBA6|client|42 MQTT User/Pass|
|42_bridge|328F2AF6A8B9A930B862A6FE|client|MQTT 42_Bridge User/Pass|
|hasadmin|CQFwKa7UF8ZHTLZr|client|Admin account for pub/sub|
|cedalo|KCY1yze1tkd4tzb-zcm|Admin broker access/managment|Cedalo Management Center access http://192.168.10x.101|


## CD&H Subsytem
### CDH Board Diagram
<p align="center">
<img src="documentation/images/CDH_v1.1_Info.png" alt=cdh_info width=600>
</p>






