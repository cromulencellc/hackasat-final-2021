


add_library(42_KIT "")

target_sources(42_KIT PRIVATE 
    ${CMAKE_CURRENT_LIST_DIR}/Source/dcmkit.c
    ${CMAKE_CURRENT_LIST_DIR}/Source/envkit.c
    ${CMAKE_CURRENT_LIST_DIR}/Source/fswkit.c
    ${CMAKE_CURRENT_LIST_DIR}/Source/geomkit.c
    ${CMAKE_CURRENT_LIST_DIR}/Source/iokit.c
    ${CMAKE_CURRENT_LIST_DIR}/Source/mathkit.c
    ${CMAKE_CURRENT_LIST_DIR}/Source/msis86kit.c
    ${CMAKE_CURRENT_LIST_DIR}/Source/nrlmsise00kit.c
    ${CMAKE_CURRENT_LIST_DIR}/Source/orbkit.c
    ${CMAKE_CURRENT_LIST_DIR}/Source/radbeltkit.c
    ${CMAKE_CURRENT_LIST_DIR}/Source/sigkit.c
    ${CMAKE_CURRENT_LIST_DIR}/Source/sphkit.c
    ${CMAKE_CURRENT_LIST_DIR}/Source/timekit.c
)


target_include_directories(42_KIT PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/Include
)

target_link_libraries(42_KIT m)