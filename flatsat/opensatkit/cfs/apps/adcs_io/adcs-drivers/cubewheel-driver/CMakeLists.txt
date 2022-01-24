cmake_minimum_required(VERSION 3.0.0)

project(cubewheel-driver 
        VERSION 0.1.0
        DESCRIPTION "Cubewheel I2C Library"
        LANGUAGES C)

include(CTest)
enable_testing()

# Add library
add_library(cubewheel-driver "")

# Add sources
target_sources(cubewheel-driver
        PUBLIC 
                ${CMAKE_CURRENT_SOURCE_DIR}/src/Cubewheel.c
)

# We need this directory, and users of our library will need it too
target_include_directories(cubewheel-driver
        PUBLIC 
                ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(cubewheel-driver adcs-driver-util i2c-wrapper gpio-wrapper gpiod)

if(ADCS_DRIVER_TESTS)
        add_subdirectory(test)
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
