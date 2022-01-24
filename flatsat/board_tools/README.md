# CDH Board Tools

Repository for tools used to program and debug CDH hardware and ZTEX USB-FPGA 2.13

Current FPGA Bitstream: Hack-a-Sat 2 /w GRETH + EDCL (8k buffer)

openMSP430_fpga.bit

## Tools Setup
All tools except ZTEX FPGA run in Docker
Usage:
```shell
source env.sh
tool_usage
```

## Build Docker Image (usually just pull from repo)
```shell
source env.sh
docker_build
```

# Tools

## GRMON:
Tool to debug and program LEON3 processor.

[Gaisler Debug Tools Websource](https://www.gaisler.com/index.php/downloads/debug-tools)

[GRMON Manual](grmon/linux/share/grmon/manual/pdf/grmon.pdf)

Download and agree to license for your use and copy package `linux` contents to `board_tools/grmon`

### Tool Macros:
1. GRMON shell. Cane be used to inspect running LEON3 processor. See GRMON Manual for more details.
2. Setup GDB server on LEON3 for debugging  (fast firmware loads)
3. Attach sparc-rtems-gdb to grmon
4. Program CDH prom file to flash so it runs on statrup 
5. Program CDH prom file to flash so it runs on startup with added flash verification (slow)

Usage (help menu):
```shell
source env.sh
grmon
```

### mkprom2:
Tool to convert LEON3 binary ELF file to ROM file to be loaded to CDH flash

[Gaisler Compilers Tools Websource](https://www.gaisler.com/index.php/downloads/compilers)

[mkprom2 Manual](mkprom2/doc/mkprom2.pdf)

Download and agree to license for your use and copy mkprom2 package contents to `board_tools/mkprom2`

Usage (help menu):
```shell
source env.sh
mkprom2
```
#### Tool Macros
1. Create prom file from ELF executable to run on LEON3

Convert core-cpu1.exe -> core-cpu1.prom

### ZTEX Tools
Tool macros to program ZTEX USB-FPGA 2.13 bitstream. Runs directly host. Need default-jre installed. 

[ZTEX Documentation](https://www.ztex.de/usb-fpga-2/usb-fpga-2.13.e.html)

[ZTEX SDK Download](https://www.ztex.de/downloads/#firmware_kit)

Download SDK and agree to license for your use and copy package `ztex` contents to `board_tools/ztex`


Usage (help menu):
```shell
./install.sh
source env.sh
ztex
```

#### Tool Macros
1. ping - Check if ZTEX FPGA is attached to host
2. program- Program bitstream to ZTEX FPGA