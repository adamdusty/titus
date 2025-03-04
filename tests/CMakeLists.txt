add_executable(sdk_tests sdk/sdk_tests.c)
target_link_libraries(sdk_tests PRIVATE titus_sdk)
add_test(NAME sdk_tests COMMAND sdk_tests)
