cmake_minimum_required(VERSION 3.10.0)

project(i2c-wrapper
        VERSION 0.1.0
        DESCRIPTION "I2C wrapper library for i2c-dev"
        LANGUAGES C)

include(CTest)
enable_testing()

add_library(i2c-wrapper "")
target_sources(i2c-wrapper PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/i2c_device.c)
target_include_directories(i2c-wrapper PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(i2c-wrapper adcs-driver-util)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
