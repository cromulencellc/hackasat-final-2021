cmake_minimum_required(VERSION 3.10.0)

project(gpio-wrapper
        VERSION 0.1.0
        DESCRIPTION "Gpio wrapper library for libgpiod"
        LANGUAGES C)

# if(NOT DEFINED CMAKE_C_COMPILER)
#         SET(CMAKE_C_COMPILER            "/usr/bin/arm-linux-gnueabihf-gcc")
# endif(NOT DEFINED CMAKE_C_COMPILER)

include(CTest)
enable_testing()

add_library(gpio-wrapper "")
target_sources(gpio-wrapper PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/gpio_device.c)
target_include_directories(gpio-wrapper PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(gpio-wrapper adcs-driver-util gpiod)

if(ADCS_DRIVER_TESTS)
        add_subdirectory(test)
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
