
# Basic cross system configuration
SET(CMAKE_SYSTEM_NAME           Linux)
SET(CMAKE_SYSTEM_VERSION        1)
SET(CMAKE_SYSTEM_PROCESSOR      armhf)

# Specify the cross compiler executables
# Typically these would be installed in a home directory or somewhere
# in /opt.  However in this example the system compiler is used.
SET(CMAKE_C_COMPILER            "/usr/bin/arm-linux-gnueabihf-gcc")
SET(CMAKE_CXX_COMPILER          "/usr/bin/arm-linux-gnueabihf-g++")


# here is the target environment located
SET(CMAKE_SYSROOT /opt/rootfs)
#SET(CMAKE_FIND_ROOT_PATH  /opt/rootfs)
#SET(CMAKE_PREFIX_PATH /opt/rootfs/usr)

# adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment, search 
# programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)

# This adds the "-m32" flag to all compile commands
#SET(CMAKE_C_FLAGS_INIT "-m32" CACHE STRING "C Flags required by platform")
SET(CMAKE_C_FLAGS_INIT "-mcpu=cortex-a9 -fPIC")
