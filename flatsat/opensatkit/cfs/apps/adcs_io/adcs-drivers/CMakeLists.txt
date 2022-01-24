cmake_minimum_required(VERSION 3.10.0)

project(adcs-driver VERSION 0.1.0 DESCRIPTION "ADCS Drivers")

include(CTest)
enable_testing()

option(ADCS_DRIVER_TESTS "Build ADCS Driver Test Code" ON)

IF( "${LOG_LEVEL}" GREATER 0 )
	#add_definitions( -DADCS_IO_LOG_LEVEL=${LOG_LEVEL} -DADCS_LOG_COLOR=1 )
    add_definitions( -DADCS_IO_LOG_LEVEL=${LOG_LEVEL} )
else()
    add_definitions( -DADCS_IO_LOG_LEVEL=0 )
endif()
add_subdirectory(adcs-driver-util)
add_subdirectory(gpio-wrapper)
add_subdirectory(i2c-wrapper)
add_subdirectory(bno08x-driver)
add_subdirectory(torquerod-driver)
add_subdirectory(cubewheel-driver)
add_subdirectory(xadc-driver)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
