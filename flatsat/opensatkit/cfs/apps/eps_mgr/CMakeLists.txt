cmake_minimum_required(VERSION 2.6.4)
project(CFS_EPS_MGR C)

add_definitions( -DEPS_IMPL_DEBUG=1 )
add_definitions( -DEPS_LOG_LEVEL=3 )

include_directories(fsw/src)
include_directories(fsw/mission_inc)
include_directories(fsw/platform_inc)
include_directories(${cfs_lib_MISSION_DIR}/fsw/public_inc)
include_directories(${adcs_io_MISSION_DIR}/fsw/public_inc)
include_directories(${adcs_io_MISSION_DIR}/adcs-drivers/adcs-driver-util/include)
include_directories(${osk_app_fw_MISSION_DIR}/fsw/platform_inc)
include_directories(${osk_app_fw_MISSION_DIR}/fsw/mission_inc)
include_directories(${osk_app_fw_MISSION_DIR}/fsw/app_inc)
include_directories(${fsw_lib_MISSION_DIR}/fsw/public_inc)

aux_source_directory(fsw/src APP_SRC_FILES)
aux_source_directory(fsw/tables APP_TABLE_FILES)

# Create the app module
add_cfe_app(eps_mgr ${APP_SRC_FILES})
target_link_libraries(eps_mgr m)
