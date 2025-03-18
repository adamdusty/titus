add_executable(sdk_tests sdk/sdk_tests.c)
target_link_libraries(sdk_tests PRIVATE titus_sdk)
add_test(NAME sdk_tests COMMAND sdk_tests)

if(WIN32)
    add_custom_command(TARGET sdk_tests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:titus_rt> .
        COMMAND_EXPAND_LISTS
    )
endif(WIN32)