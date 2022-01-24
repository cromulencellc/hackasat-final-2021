cmake_minimum_required(VERSION 3.0.0)

project(bno08x-driver 
        VERSION 0.1.0
        DESCRIPTION "BNO08x I2C Library"
        LANGUAGES C)

include(CTest)
enable_testing()

# Add library
add_library(bno08x-driver "")

# Add sources
target_sources(bno08x-driver
        PUBLIC 
                ${CMAKE_CURRENT_SOURCE_DIR}/src/BNO08x_Driver.c
        PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}/sh2/sh2_SensorValue.c
                ${CMAKE_CURRENT_SOURCE_DIR}/sh2/sh2_util.c
                ${CMAKE_CURRENT_SOURCE_DIR}/sh2/sh2.c
                ${CMAKE_CURRENT_SOURCE_DIR}/sh2/shtp.c
)

# We need this directory, and users of our library will need it too
target_include_directories(bno08x-driver
        PUBLIC 
                ${CMAKE_CURRENT_SOURCE_DIR}/include
                ${CMAKE_CURRENT_SOURCE_DIR}/sh2
)

target_link_libraries(bno08x-driver adcs-driver-util i2c-wrapper gpio-wrapper gpiod m)

if(ADCS_DRIVER_TESTS)
        add_subdirectory(test)
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
