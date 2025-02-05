add_executable(titus_integration_tests src/main.c)
add_test(NAME integration COMMAND titus_integration_tests)

add_custom_command(
    TARGET titus_integration_tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/modules
        ${CMAKE_CURRENT_BINARY_DIR}/modules
)