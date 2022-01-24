cmake_minimum_required(VERSION 2.6.4)
project(CFS_SLA_TLM C)

add_definitions(-D_ATTRIBUTION_KEY_=${_ATTRIBUTION_KEY_})
include_directories(fsw/src)
include_directories(fsw/mission_inc)
include_directories(fsw/platform_inc)
include_directories(${cfs_lib_MISSION_DIR}/fsw/public_inc)
include_directories(${osk_app_fw_MISSION_DIR}/fsw/platform_inc)
include_directories(${osk_app_fw_MISSION_DIR}/fsw/mission_inc)
include_directories(${osk_app_fw_MISSION_DIR}/fsw/app_inc)

aux_source_directory(fsw/src APP_SRC_FILES)
aux_source_directory(fsw/tables APP_TABLE_FILES)

# Create the app module
add_cfe_app(sla_tlm ${APP_SRC_FILES})
