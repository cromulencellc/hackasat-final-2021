
add_executable(test-xadc-driver test_xadc_driver.c)

target_include_directories(test-xadc-driver
        PUBLIC 
                ${PROJECT_SOURCE_DIR}/include

)

target_link_libraries(test-xadc-driver xadc-driver)

