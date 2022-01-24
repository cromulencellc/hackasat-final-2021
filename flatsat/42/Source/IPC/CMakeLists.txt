
add_library(SIM_IPC "")

target_sources(SIM_IPC PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}/SimReadFromFile.c
    ${CMAKE_CURRENT_LIST_DIR}/SimWriteToFile.c
    ${CMAKE_CURRENT_LIST_DIR}/SimReadFromSocket.c
    ${CMAKE_CURRENT_LIST_DIR}/SimWriteToSocket.c
)

target_include_directories(SIM_IPC
    PRIVATE
        ${PROJECT_SOURCE_DIR}/Include
        ${PROJECT_SOURCE_DIR}/Kit/Include
)

target_link_libraries(SIM_IPC m)

add_library(APP_IPC "")

target_sources(APP_IPC PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}/AppReadFromFile.c
    ${CMAKE_CURRENT_LIST_DIR}/AppWriteToFile.c
    ${CMAKE_CURRENT_LIST_DIR}/AppReadFromSocket.c
    ${CMAKE_CURRENT_LIST_DIR}/AppWriteToSocket.c
)

target_include_directories(APP_IPC
    PRIVATE
        ${PROJECT_SOURCE_DIR}/Include
        ${PROJECT_SOURCE_DIR}/Kit/Include
)

target_link_libraries(APP_IPC m)