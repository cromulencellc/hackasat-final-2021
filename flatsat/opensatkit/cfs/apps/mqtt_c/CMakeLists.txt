cmake_minimum_required(VERSION 2.6.4)
project(CFS_MQTT_C C)

if ("${MISSIONCONFIG}" STREQUAL "osk_cdh")
add_definitions(-DMQTT_RTEMS)
endif()

set(MQTT_C_TESTS OFF CACHE BOOL "Don't build MQTT_C Tests")
set(MQTT_C_EXAMPLES CACHE BOOL "Don't build MQTT_C Examples")
add_subdirectory(MQTT-C)

add_definitions( -DMQTT_LOG_LEVEL=4 )

include_directories(fsw/mission_inc)
include_directories(fsw/platform_inc)
include_directories(fsw/src)
include_directories(MQTT-C/include)

#include_directories(${osk_c_fw_MISSION_DIR}/fsw/app_inc)
#include_directories(${osk_c_fw_MISSION_DIR}/fsw/platform_inc)
#include_directories(${osk_c_fw_MISSION_DIR}/fsw/mission_inc)
include_directories(${osk_app_fw_MISSION_DIR}/fsw/platform_inc)
include_directories(${osk_app_fw_MISSION_DIR}/fsw/mission_inc)
include_directories(${osk_app_fw_MISSION_DIR}/fsw/app_inc)
include_directories(${cfs_lib_MISSION_DIR}/fsw/public_inc)
include_directories(${mqtt_c_lib_MISSION_DIR}/fsw/public_inc)


aux_source_directory(fsw/src APP_SRC_FILES)
# aux_source_directory(fsw/tables APP_TABLE_FILES)

# if ("${MISSIONCONFIG}" STREQUAL "osk_cdh")
#     add_definitions(-DMQTTCLIENT_PLATFORM_HEADER=MQTTRtems.h)
#     include_directories(${mqtt_lib_MISSION_DIR}/fsw/platform_inc_rtems)
# else()
#     add_definitions(-DMQTTCLIENT_PLATFORM_HEADER=MQTTLinux.h)
#     include_directories(${mqtt_lib_MISSION_DIR}/fsw/platform_inc)
# endif()

# Create the app module
add_cfe_app(mqtt_c ${APP_SRC_FILES})
target_link_libraries(mqtt_c mqttc)
