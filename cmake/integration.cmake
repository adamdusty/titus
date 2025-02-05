set(INTEGRATION_PATH "${CMAKE_CURRENT_BINARY_DIR}/integration/")

add_custom_target(integration ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${INTEGRATION_PATH}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${INTEGRATION_PATH}/modules/core/rend/v1.3.2
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:titus_rt> ${INTEGRATION_PATH}
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:titus_rt> ${INTEGRATION_PATH}
    COMMAND_EXPAND_LISTS
)
add_dependencies(integration titus_rt)
