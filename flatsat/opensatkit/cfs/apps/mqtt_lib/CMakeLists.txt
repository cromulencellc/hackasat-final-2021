cmake_minimum_required(VERSION 2.6.4)
project(CFE_MQTT_LIB C)

add_definitions( -DMQTT_LOG_LEVEL=3 )

include_directories(fsw/public_inc)

# The shared OSAL and cFE include directories should always be used
# Note that this intentionally does NOT include PSP-specific includes, just the generic
include_directories(${CFECORE_SOURCE_DIR}/src/inc)
include_directories(${CFEPSP_SOURCE_DIR}/fsw/inc)

aux_source_directory(fsw/src LIB_SRC_FILES)

if ("${MISSIONCONFIG}" STREQUAL "osk_cdh")
    add_definitions(-DMQTTCLIENT_PLATFORM_HEADER=MQTTRtems.h)
    include_directories(fsw/platform_inc_rtems)
    list(APPEND LIB_SRC_FILES "fsw/platform_inc_rtems/MQTTRtems.c")
else()
    add_definitions(-DMQTTCLIENT_PLATFORM_HEADER=MQTTLinux.h)
    include_directories(fsw/platform_inc)
    list(APPEND LIB_SRC_FILES "fsw/platform_inc/MQTTLinux.c")
endif()

# Create the app module
add_cfe_app(mqtt_lib ${LIB_SRC_FILES})

