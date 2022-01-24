# ZYNQ ADCS Vivado Project

## Vivado Installation 
Assumptions:
- Ubuntu 18.04 VM with at least 120GB free space
- VM is connected to internet

1. Download Xilinx_Unified_2020.2_1118_1232_Lin64.bin Xilinx
2. Run commands below to unpack installer and get authentication token
```bash
./Xilinx_Unified_2020.1_0602_1208_Lin64.bin --noexec --target /home/$USER/xilinx-installer
./xsetup -b AuthTokenGen
```
3. Run config gen for install with Zynq 7000
```bash
./xsetup -b ConfigGen
```
- Choose 1 Vitis Suite (includes Vitis SDK and Vivado)
- Set All Modules on Modules line to 0 except Zynq-7000 (this makes download smaller)
4. Run install in batch mode
```bash
./xsetup --batch Install --agree XilinxEULA,3rdPartyEULA,WebTalkTerms --location /home/$USER/Xilinx/ --config /home/$USER/.Xilinx/install_config.txt
```
5. Drink Coffee while you wait for install to complete

## Project Setup/Use
1. Create project directory
```shell
mkdir -p /home/${USER}/adcs-zynq
```
2. Start Vivado 2020.2
3. Open TCL console at bottom of screen
4. cd /home/${USER}/adcs-zync
5. Run TCL script to rebuild project
```tcl
source rebuild.tcl
```
6. Project should regenerate block diagram and be good to go
- Test by running Generate Bitstream (Runs Implementation and Synthethis)
7. Make updates as need to FPGA design
8. Export HW from Vivado
    a. File -> Export -> Export Hardware -> Choose directory /home/${USER}/adcs-zynq/hw (include bitstream)
    b. File -> Export -> Export Bitstream -> Choose /home/${USER}/adcs-zynq/hw/adcs_bd_wrapper.bit
8. Update rebuild.tcl
    a. Open TCL Console
    b. Change directory to repository folder and run write_project_tcl script
    ```tcl
    cd /home/${USER}/adcs-zynq
    write_project_tcl rebuild -force
    ```
    c. Verify that an updated rebuild.tcl file was created.
9. Add changes to Git
10. Commit and Push updates

