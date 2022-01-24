

cmake_minimum_required(VERSION 3.10)

project(42 VERSION 0.1.0 DESCRIPTION "42")

set(CMAKE_C_COMPILER /usr/bin/gcc)
# SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O1")
#-Wall -Wshadow -Wno-deprecated

set(GUI_FLAG -D_USE_GUI_)
set(SHADER_FLAG -D_USE_SHADERS_)
set(GSFCFLAG -D_USE_GSFC_WATERMARK_)
set(STANDALONE_FLAG)
set(GLFWFLAG -D_USE_GLFW_)
set(MQTT_FLAG -D_ENABLE_MQTT_)

# add_subdirectory(Source/IPC)
# add_subdirectory(Kit)

set(GUI_SOURCES)
set(GUI_LIBS)
set(GUI_INC)
if(DEFINED GUI_FLAG)
    list(APPEND GUI_SOURCES
        ${PROJECT_SOURCE_DIR}/Source/42GlutGui.c
        ${PROJECT_SOURCE_DIR}/Kit/Source/glkit.c
    )
    list(APPEND GUI_LIBS
        glut GLU GL
    )
    list(APPEND GUI_INC
        ${PROJECT_SOURCE_DIR}/Include
        ${PROJECT_SOURCE_DIR}/Kit/Include
    )
endif()


set(MQTT_LIBS)
if(DEFINED MQTT_FLAG)
    list(APPEND MQTT_LIBS
        paho-mqtt3c
    )
endif()

add_executable(42 "")

target_compile_definitions(42 PRIVATE ${GUI_FLAG} ${SHADER_FLAG} ${MQTT_FLAG} ${GSFCFLAG} ${GLFWFLAG} )

target_sources(42 
    PRIVATE
        ${PROJECT_SOURCE_DIR}/Source/42main.c
        ${PROJECT_SOURCE_DIR}/Source/42exec.c
        ${PROJECT_SOURCE_DIR}/Source/42actuators.c
        ${PROJECT_SOURCE_DIR}/Source/42cmd.c
        ${PROJECT_SOURCE_DIR}/Source/42dynamics.c
        ${PROJECT_SOURCE_DIR}/Source/42environs.c
        ${PROJECT_SOURCE_DIR}/Source/42ephem.c
        ${PROJECT_SOURCE_DIR}/Source/42fsw.c
        ${PROJECT_SOURCE_DIR}/Source/42init.c
        ${PROJECT_SOURCE_DIR}/Source/42ipc.c
        ${PROJECT_SOURCE_DIR}/Source/42perturb.c
        ${PROJECT_SOURCE_DIR}/Source/42report.c
        ${PROJECT_SOURCE_DIR}/Source/42sensors.c
        ${PROJECT_SOURCE_DIR}/Source/42nos3.c
        ${PROJECT_SOURCE_DIR}/Source/AcApp.c
        ${PROJECT_SOURCE_DIR}/Source/IPC/SimReadFromFile.c
        ${PROJECT_SOURCE_DIR}/Source/IPC/SimWriteToFile.c
        ${PROJECT_SOURCE_DIR}/Source/IPC/SimReadFromSocket.c
        ${PROJECT_SOURCE_DIR}/Source/IPC/SimWriteToSocket.c
        ${PROJECT_SOURCE_DIR}/Source/IPC/SimReadFromMqtt.c
        ${PROJECT_SOURCE_DIR}/Source/IPC/SimWriteToMqtt.c
        ${PROJECT_SOURCE_DIR}/Kit/Source/dcmkit.c
        ${PROJECT_SOURCE_DIR}/Kit/Source/envkit.c
        ${PROJECT_SOURCE_DIR}/Kit/Source/fswkit.c
        ${PROJECT_SOURCE_DIR}/Kit/Source/geomkit.c
        ${PROJECT_SOURCE_DIR}/Kit/Source/iokit.c
        ${PROJECT_SOURCE_DIR}/Kit/Source/mathkit.c
        ${PROJECT_SOURCE_DIR}/Kit/Source/msis86kit.c
        ${PROJECT_SOURCE_DIR}/Kit/Source/nrlmsise00kit.c
        ${PROJECT_SOURCE_DIR}/Kit/Source/orbkit.c
        ${PROJECT_SOURCE_DIR}/Kit/Source/radbeltkit.c
        ${PROJECT_SOURCE_DIR}/Kit/Source/sigkit.c
        ${PROJECT_SOURCE_DIR}/Kit/Source/sphkit.c
        ${PROJECT_SOURCE_DIR}/Kit/Source/timekit.c
        ${GUI_SOURCES}
)

target_include_directories(42
    PUBLIC
        ${PROJECT_SOURCE_DIR}/Include
        ${PROJECT_SOURCE_DIR}/Kit/Include
    PRIVATE
        ${GUI_INC}
)

target_link_libraries(42 ${GUI_LIBS} ${MQTT_LIBS} dl m)

# add_executable(FlatSatFswApp "")

# target_compile_definitions(FlatSatFswApp PRIVATE ${GUI_FLAG} ${SHADER_FLAG} ${MQTT_FLAG} ${GSFCFLAG} ${GLFWFLAG} ${FLATSATFSWFLAG})

# target_sources(FlatSatFswApp
#     PRIVATE
#         ${PROJECT_SOURCE_DIR}/Source/FlatSatFsw.c
#         ${PROJECT_SOURCE_DIR}/Source/IPC/AppReadFromFile.c
#         ${PROJECT_SOURCE_DIR}/Source/IPC/AppWriteToFile.c
#         ${PROJECT_SOURCE_DIR}/Source/IPC/AppReadFromSocket.c
#         ${PROJECT_SOURCE_DIR}/Source/IPC/AppWriteToSocket.c
#         ${PROJECT_SOURCE_DIR}/Source/IPC/AppReadFromMqtt.c
#         ${PROJECT_SOURCE_DIR}/Source/IPC/AppWriteToMqtt.c
#         ${PROJECT_SOURCE_DIR}/Kit/Source/dcmkit.c
#         ${PROJECT_SOURCE_DIR}/Kit/Source/fswkit.c
#         ${PROJECT_SOURCE_DIR}/Kit/Source/iokit.c
#         ${PROJECT_SOURCE_DIR}/Kit/Source/mathkit.c
#         ${PROJECT_SOURCE_DIR}/Kit/Source/orbkit.c
#         ${PROJECT_SOURCE_DIR}/Kit/Source/timekit.c
# )

# target_include_directories(FlatSatFswApp
#     PUBLIC
#         ${PROJECT_SOURCE_DIR}/Include
#         ${PROJECT_SOURCE_DIR}/Kit/Include
# )

# target_link_libraries(FlatSatFswApp ${GUI_LIBS} ${MQTT_LIBS} m)

install(TARGETS 42 DESTINATION ${PROJECT_SOURCE_DIR})

