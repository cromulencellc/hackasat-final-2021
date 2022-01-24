cmake_minimum_required(VERSION 2.6.4)
project(CFS_SBN_LITE C)

include_directories(fsw/mission_inc)
if ("${MISSIONCONFIG}" STREQUAL "osk_adcs")
    include_directories(fsw/platform_inc_adcs)
else()
    include_directories(fsw/platform_inc)
endif()

add_definitions( -DTEAM_NUM=${TEAM_NUM} )

#include_directories(fsw/platform_inc)
include_directories(fsw/src)
include_directories(./../inc)
include_directories(${cfs_lib_MISSION_DIR}/fsw/public_inc)
include_directories(${cfs_lib_PLATFORM_DIR}/fsw/public_inc)
include_directories(${osk_app_fw_MISSION_DIR}/fsw/platform_inc)
include_directories(${osk_app_fw_MISSION_DIR}/fsw/mission_inc)
include_directories(${osk_app_fw_MISSION_DIR}/fsw/app_inc)
include_directories(${expat_lib_MISSION_DIR}/fsw/platform_inc)
include_directories(${io_lib_MISSION_DIR}/fsw/public_inc)

include_directories(${cf_MISSION_DIR}/fsw/src)
include_directories(${cf_MISSION_DIR}/fsw/mission_inc)
if ("${MISSIONCONFIG}" STREQUAL "osk_adcs")
    include_directories(${cf_MISSION_DIR}/fsw/platform_inc_adcs)
else()
    include_directories(${cf_MISSION_DIR}/fsw/platform_inc)
endif()

aux_source_directory(fsw/src APP_SRC_FILES)
aux_source_directory(fsw/tables APP_TABLE_FILES)

# Create the app module
add_cfe_app(sbn_lite ${APP_SRC_FILES})

