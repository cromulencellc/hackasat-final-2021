
add_executable(test-gpio-driver test_gpio.c)

target_include_directories(test-gpio-driver
        PUBLIC 
                ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(test-gpio-driver gpio-wrapper gpiod)

