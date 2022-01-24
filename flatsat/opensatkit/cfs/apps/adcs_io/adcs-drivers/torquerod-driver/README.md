# torquerod-driver

Linux Driver for magnetic torque rod device driver using two GPIO pins and a H-Bridge circuit used on ADCS.

## Setup

This project can be built in a couple different ways.
1. Build natively directly on linux running on Zynq SOC. Primarily used for development and testing.
2. Build cross platform targing ARMv7 on x86 in Ubuntu. Primarily used for development and testing.
3. Build as part of CFS application. This is also a cross platform build, but packaging is different.

## Linux Dependencies

```bash
sudo apt install libgpiod
```

# Build Instructions

Project can built indepdently using Makefile wrapper and CMake. Builds with Debug symbols since this is for testing.

```bash
make
```

Project can also be built using native CFS tools since CFS is a CMake project. See CFS build instructions.