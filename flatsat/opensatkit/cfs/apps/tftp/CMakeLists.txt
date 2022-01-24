cmake_minimum_required(VERSION 2.6.4)
project(CFS_TFTP C)

include_directories(fsw/mission_inc)
if("${MISSIONCONFIG}" STREQUAL "osk_adcs")
    include_directories(fsw/platform_inc_adcs)
else()
    include_directories(fsw/platform_inc)
endif()
include_directories(${osk_app_fw_MISSION_DIR}/fsw/platform_inc)
include_directories(${osk_app_fw_MISSION_DIR}/fsw/mission_inc)
include_directories(${osk_app_fw_MISSION_DIR}/fsw/app_inc)

aux_source_directory(fsw/src APP_SRC_FILES)

# Create the app module
add_cfe_app(tftp ${APP_SRC_FILES})

