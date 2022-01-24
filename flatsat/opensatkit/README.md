# Hack-a-Sat 2 Opensatkit Baseline

The HAS2 Flight Software and Ground System is based on an open source project called OpenSatKit.

OpenSatKit provides a complete desktop solution for learning how to use NASA's open source flight software (FSW) platform called the core Flight System (cFS). The cFS is a reusable FSW architecture that provides a portable and extendable platform with a product line deployment model. 

The cFS deployment has been tailored specifically to the HAS2 project based on the following changes:

* Addition osk_cdh mission targeting LEON3 / RTEMS 5 
* Addition osk_adcs mission targeting ARM / Linux (Deployment to Zynq 7010 running linux)
* Development of a custom Command Ingest / Telemetry Output Application that interfaces to a LEON3 apbuart
* Development of a custom Eyassat IF application that interfaces to the sensors and actuators provided by the EyasSat training satellite
* Development of a custom PL IF application that can be used in conjunction with the RPI Zero from the Cromulence C&DH board to take/downlink pictures 

The Cosmos install provided as part of Opensatkit has been tailored specifically to the HAS2 project based on the following changes:

* Addition of targets to support HAS2 custom cFS applications
* Screen updates to support HAS2 custom cFS applications

## Prerequisites (Minimum)
* Linux host with Docker installed
* Linux host with running Xserver (COSMOS)

## CFS
### Build cFS

Build using Docker (e.g. build osk_cdh mission for team 1)
```shell
cd cfs
./build.sh -m osk_cdh -t 1
```

Build for all teams all missions
``` shell
./build.sh -a
```

* Missions included:
    * osk = x86/Linux
    * osk_rtems = i386/RTEMS5
    * osk_cdh = LEON3/RTEMS5
    * osk_adcs = ARM/Linux


## COSMOS
### Run Cosmos in Docker (Preferred)
- Uses has2/finals/opensatkit-docker/cosmos:latest image running Cosmos 4.5.1

```shell
cd cosmos
./run.sh
```

## Install Cosmos Local

```shell
cd cosmos
./setup.sh
rm Gemfile.lock
bundle install
```