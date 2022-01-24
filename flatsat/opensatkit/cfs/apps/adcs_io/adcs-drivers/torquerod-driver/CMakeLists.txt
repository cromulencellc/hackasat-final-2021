cmake_minimum_required(VERSION 3.10.0)

project(torquerod-driver
        VERSION 0.1.0
        DESCRIPTION "Magnetic Torque Rod Control Library"
        LANGUAGES C)

include(CTest)
enable_testing()

# Add library
add_library(torquerod-driver "")

# Add sources
target_sources(torquerod-driver
        PUBLIC 
                ${CMAKE_CURRENT_SOURCE_DIR}/src/TorqueRod.c
)

# We need this directory, and users of our library will need it too
target_include_directories(torquerod-driver 
        PUBLIC 
                ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(torquerod-driver adcs-driver-util gpio-wrapper gpiod)

if(ADCS_DRIVER_TESTS)
        add_subdirectory(test)
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
