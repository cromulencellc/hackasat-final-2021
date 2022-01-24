
add_executable(test-cubewheel test_cubewheel.c)

target_include_directories(test-cubewheel
        PUBLIC 
                ${PROJECT_SOURCE_DIR}/include

)

target_link_libraries(test-cubewheel cubewheel-driver adcs-driver-util i2c-wrapper gpio-wrapper gpiod)

