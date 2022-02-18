Distribution Statement A: Approved for Public Release AFRL-2022-0566 
# Hack-a-Sat 2 -  2021 Final #
---

This repository contains the open source release for the Hack-a-Sat 2 2021
final event.

Released artifacts include:

* Source code for all challenges
* Source code for all challenge solutions
* Source code for flatsat software and tools
* Source code for custom hardware boards
* Source code for custom cFS and COSMOS deployments

Released artifacts *do not* include:

* Infrastructure used to host and run the game
* Source code for the score board
* Hardware schematics for the custom boards
* Hardware drawings for flatsat enclosures


## Repository Structure ##

* `cdh_rev2` - ztex FPGA project
* `flatsat_openmsp430` - OpenMSP430 core ised to control LEON3 processor
* `flatsat` - Repository to setup, manage, and control HAS2 flatsat hardware
  * `42` NASA open source space simulation tool customized for HAS2
  * `board_tools` Stub respository for devlopment tools for cdh
  * `comms` GNU radio flowgraphs for user segment and comm payload SDRs and supporting softwre. Includes source for challenge 3 and challenge 4.
  * `fax` Server side code for challenge 5 using Cosmos RPC APIs
  * `fortytwo-mqtt-bridge` Translate 42 IPC to MQTT
  * `mosquitto` MQTT server configuration
  * `opensatkit` Contains CFS source code and Cosmos ground station project. Includes source for challenge 5 and challenge 7.
* `challenges` - Solutions for challenges 1-7.


## License ##

Files in this repository are provided as-is under the MIT license unless
otherwise stated below or by a license header. See [LICENSE.md](LICENSE.md)
for more details.

42 and cFS are provided under the NOSA v1.3 license.

COSMOS is provided under the GPLv3 license.

OSK is provided under the LGPL license.

GRLIB is provided under the GPL license.

openMSP430 is provided under the BSD license.

packer-builder-arm is provided under the Apache license.

MQTT-C is provided under the MIT license.

mqtt_lib includes lib_paho which is provided under the Apache license.


## Contact ##

Questions, comments, or concerns can be sent to `hackasat[at]cromulence.com`.
