
add_executable(test-torquerod-driver test_torquerod.c)

target_include_directories(test-torquerod-driver
        PUBLIC 
                ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(test-torquerod-driver torquerod-driver)

