cmake_minimum_required(VERSION 2.6.4)
project(CFE_ADCS_IO_LIB)

# Add ADCS HW Driver library to build
set(LOG_LEVEL 2)

set(ADCS_DRIVER_TESTS OFF CACHE BOOL "Don't ADCS Drivers Tests")
add_subdirectory(adcs-drivers)

include_directories(fsw/public_inc)
# The shared OSAL and cFE include directories should always be used
# Note that this intentionally does NOT include PSP-specific includes, just the generic
include_directories(${CFECORE_SOURCE_DIR}/src/inc)
include_directories(${CFEPSP_SOURCE_DIR}/fsw/inc)

# adcs-drivers public headers for interfaces
include_directories(adcs-drivers/adcs-driver-util/include)
include_directories(adcs-drivers/bno08x-driver/include)
include_directories(adcs-drivers/cubewheel-driver/include)
include_directories(adcs-drivers/gpio-wrapper/include)
include_directories(adcs-drivers/i2c-wrapper/include)
include_directories(adcs-drivers/torquerod-driver/include)
include_directories(adcs-drivers/xadc-driver/include)

aux_source_directory(fsw/src LIB_SRC_FILES)

# Create the app module
add_cfe_app(adcs_io_lib ${LIB_SRC_FILES})
target_link_libraries(adcs_io_lib adcs-driver-util torquerod-driver bno08x-driver cubewheel-driver xadc-driver)
