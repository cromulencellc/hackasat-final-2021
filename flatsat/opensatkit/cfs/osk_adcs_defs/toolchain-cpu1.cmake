# This example toolchain file describes the cross compiler to use for
# the target architecture indicated in the configuration file.

# In this sample application, the "cross" toolchain is configured to
# simply use the system native compiler with the "m32" switch to output
# 32-bit code on a 64-bit system.  This will not be necessary in 
# future revisions.

# Basic cross system configuration
SET(CMAKE_SYSTEM_NAME           Linux)
SET(CMAKE_SYSTEM_VERSION        1)
SET(CMAKE_SYSTEM_PROCESSOR      armhf)

# Specify the cross compiler executables
# Typically these would be installed in a home directory or somewhere
# in /opt.  However in this example the system compiler is used.
SET(CMAKE_C_COMPILER            "/usr/bin/arm-linux-gnueabihf-gcc")
SET(CMAKE_CXX_COMPILER          "/usr/bin/arm-linux-gnueabihf-g++")

SET(CMAKE_SYSROOT /opt/rootfs)

# Configure the find commands
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)

# These variable settings are specific to cFE/OSAL and determines which 
# abstraction layers are built when using this toolchain
SET(CFE_SYSTEM_PSPNAME      "pc-linux")
SET(OSAL_SYSTEM_BSPNAME     "pc-linux")
SET(OSAL_SYSTEM_OSTYPE      "posix")

# This adds the "-m32" flag to all compile commands
#SET(CMAKE_C_FLAGS_INIT "-m32" CACHE STRING "C Flags required by platform")
SET(CMAKE_C_FLAGS_INIT "-mcpu=cortex-a9 -fPIC")
# SET(CMAKE_C_FLAGS_INIT "-fsigned-char")
