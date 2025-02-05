add_subdirectory(minimal)

add_custom_target(examples ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory "modules/${MINIMAL_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:minimal> "modules/${MINIMAL_DIR}/"
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:titus_rt> ${CMAKE_CURRENT_BINARY_DIR}
    COMMAND_EXPAND_LISTS
)

configure_file(minimal/module.json "modules/${MINIMAL_DIR}/")

add_dependencies(examples titus_rt minimal)

if(WIN32)
add_custom_command(TARGET examples POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:titus_rt> ${CMAKE_CURRENT_BINARY_DIR}
    COMMAND_EXPAND_LISTS
)
endif(WIN32)