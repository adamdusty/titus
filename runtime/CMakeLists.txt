add_executable(titus_rt)
target_sources(titus_rt
    PRIVATE
        src/main.cpp
)

target_link_libraries(titus_rt PRIVATE titus::sdk)

set_target_properties(
    titus_rt PROPERTIES
    OUTPUT_NAME titus
)

add_custom_command(TARGET titus_rt POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    $<TARGET_RUNTIME_DLLS:titus_rt>
    $<TARGET_FILE_DIR:titus_rt>
    COMMAND_EXPAND_LISTS
)