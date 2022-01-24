cmake_minimum_required(VERSION 3.10.0)

project(adcs-driver-util
        VERSION 0.1.0
        DESCRIPTION "ADCS Driver Common Utils"
        LANGUAGES C)

include(CTest)
enable_testing()

add_library(adcs-driver-util "")
target_sources(adcs-driver-util 
        PUBLIC 
                ${CMAKE_CURRENT_SOURCE_DIR}/src/adcs_driver_util.c
)

target_include_directories(adcs-driver-util 
        PUBLIC 
                ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(adcs-driver-util m)

# add_subdirectory(test)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
