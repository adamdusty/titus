add_subdirectory(minimal)
add_subdirectory(testingground)

add_custom_target(examples ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory "modules/minimal"
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:minimal> "modules/minimal/"
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:titus_rt> ${CMAKE_CURRENT_BINARY_DIR}
    COMMAND_EXPAND_LISTS
)
add_dependencies(examples titus_rt minimal)

file(COPY minimal/module.json DESTINATION modules/minimal/)


if(WIN32)
add_custom_command(TARGET examples POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:titus_rt> ${CMAKE_CURRENT_BINARY_DIR}
    COMMAND_EXPAND_LISTS
)
endif(WIN32)