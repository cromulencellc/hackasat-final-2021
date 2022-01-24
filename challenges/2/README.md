# Challenge 2 -  EPS Management

## Description
For this challenge teams must figure out how to turn on the spacecraft main communications payload. This payload being in the "ON" state is requied for future challenges.

Teams must analyze the EPS_MGR teletry and figure out that for all power modes the PAYLOAD comment is never moded into the "ON" state. 

Teams can then either reverse engineer the spacecraft binary for the EPS_MGR application, try downloading configuration tables, or perform other investigation to narrow the problem down the the EPS_MGR "eps_mgs_cfg_tlb.json" configuration file. This file is loaded on startup by the EPS_MGR application and is used to control which subsystems, payloads, and spacecraft devices are allowed to be in the "ON" state for each mode. Additionally the default state for each component in each mode is set by the configuration table as well. 

The important configuration value is the "mode-mask" config value in the JSON formatted configuration table.

## Solver
The solver downloads the existing EPS_MGR configuration table from the spacecraft by first requesting the EPS_MGR dump its config to file, then downloading that file using CFDP.

Next the file would be analyzed and teams would have to figure out that the "mode-mask" is a simple unsigned integer with a bit mask where each bit represents that state of a different component, starting with the LSB. After analyis teams should find the relationship below.

|Bit 8|Bit 7|Bit 6|Bit 5|Bit 4|Bit 3|Bit 3|Bit 1|Bit 0 (LSB|
|---|---|---|---|---|---|---|---|---|
|Comm Payload|Fine Sun Sensor|Coarse Sun Sensor|Magnetic Torque Rod|Star Tracker|IMU|Reaction Wheel|ADCS|TTC Comm|

So once this is found, the "mode-mask" for the "NOMINAL_OPS_PAYLOAD_ON" mode can be update to a decimal value of 351 from a broken value.

Solver then uploads fixed table and loads it into the EPG_MGR application.

Then the EPS_MGR can be set to the "NOMINAL_OPS_PAYLOAD_ON" mode successfully and the payload will turn on. 

Solver implements this functionality using Cosmos ruby scripts.

The solver can be run from the Cosmos script runner plugin.