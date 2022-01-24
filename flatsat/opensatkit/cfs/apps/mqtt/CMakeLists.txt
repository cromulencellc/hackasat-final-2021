cmake_minimum_required(VERSION 2.6.4)
project(CFS_MQTT C)

add_definitions( -DMQTT_LOG_LEVEL=3)

include_directories(fsw/mission_inc)
include_directories(fsw/platform_inc)
include_directories(fsw/src)
#include_directories(${osk_c_fw_MISSION_DIR}/fsw/app_inc)
#include_directories(${osk_c_fw_MISSION_DIR}/fsw/platform_inc)
#include_directories(${osk_c_fw_MISSION_DIR}/fsw/mission_inc)
include_directories(${osk_app_fw_MISSION_DIR}/fsw/platform_inc)
include_directories(${osk_app_fw_MISSION_DIR}/fsw/mission_inc)
include_directories(${osk_app_fw_MISSION_DIR}/fsw/app_inc)
include_directories(${cfs_lib_MISSION_DIR}/fsw/public_inc)
include_directories(${mqtt_lib_MISSION_DIR}/fsw/public_inc)
# include_directories(${adcs_io_MISSION_DIR}/fsw/public_inc)



aux_source_directory(fsw/src APP_SRC_FILES)
# aux_source_directory(fsw/tables APP_TABLE_FILES)

if ("${MISSIONCONFIG}" STREQUAL "osk_cdh")
    add_definitions(-DMQTTCLIENT_PLATFORM_HEADER=MQTTRtems.h)
    include_directories(${mqtt_lib_MISSION_DIR}/fsw/platform_inc_rtems)
else()
    add_definitions(-DMQTTCLIENT_PLATFORM_HEADER=MQTTLinux.h)
    include_directories(${mqtt_lib_MISSION_DIR}/fsw/platform_inc)
endif()

# Create the app module
add_cfe_app(mqtt ${APP_SRC_FILES})
# add_cfe_tables(mqtt ${APP_TABLE_FILES})
