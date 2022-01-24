
add_executable(test-bno08x-driver test_bno08x.c)

target_include_directories(test-bno08x-driver
        PUBLIC 
                ${PROJECT_SOURCE_DIR}/include
                ${PROJECT_SOURCE_DIR}/sh2
)

target_link_libraries(test-bno08x-driver bno08x-driver pthread)

add_executable(bno08x_cal bno08x_cal.c)

target_include_directories(bno08x_cal
        PUBLIC 
                ${PROJECT_SOURCE_DIR}/include
                ${PROJECT_SOURCE_DIR}/sh2
)

target_link_libraries(bno08x_cal bno08x-driver)