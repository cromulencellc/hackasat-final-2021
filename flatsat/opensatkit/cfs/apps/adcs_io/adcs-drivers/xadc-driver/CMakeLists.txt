cmake_minimum_required(VERSION 3.0.0)

project(xadc-driver 
        VERSION 0.1.0
        DESCRIPTION "XADC Libiio Library"
        LANGUAGES C)

include(CTest)
enable_testing()

# Add library
add_library(xadc-driver "")

# Add sources
target_sources(xadc-driver
        PUBLIC 
                ${CMAKE_CURRENT_SOURCE_DIR}/src/XadcDriver.c
)

# We need this directory, and users of our library will need it too
target_include_directories(xadc-driver
        PUBLIC 
                ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(xadc-driver adcs-driver-util iio)

if(ADCS_DRIVER_TESTS)
        add_subdirectory(test)
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
