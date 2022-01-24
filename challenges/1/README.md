# Challenge 1 -  Recover Spacraft

## Description
When the CTF starts each teams spacecraft starts in a non-nominal power mode that results in the spacecraft tumbling out-of-control.

This mode is called SEPERATION and refers to the limited power state a spacecraft would operate in after seperation from a launch vehicle.

Teams must analyze telemetry and figure out why the spacecraft can't control its attidue and apply a fix. 

## Solver
Solver is a Cosmos ruby script that sends the SET_MODE command to the EPS_MGR target with the update MODE value SAFE.

This causes the command and control system (Cosmos) to send a command to the EPS Manager CFS application to set the EPS system mode to "SAFE".

This mode has the ADCS components required for pointing and control in the "ON" state (wheels, sensors, etc.)

Once this command is sent the solver verifies the mode transition occurs successfully in telemetry.

Solver is run from the Cosmos script runner plugin.


## Notes 
This challenge is meant to be a simple challenge where teams are supposed to look around spacecraft telemetry and commands to get a better understanding of the spacecraft they are tasked with controlling.