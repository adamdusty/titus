# Creates target with all the necessary components for the runtime

add_custom_target(titus ALL
   COMMAND ${CMAKE_COMMAND} -E make_directory "modules"
   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:titus_rt> .
)

if(WIN32)
    add_custom_command(TARGET titus POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:titus_rt> .
        COMMAND_EXPAND_LISTS
    )
endif(WIN32)

function(ADD_MODULE module_name module_target)
    add_custom_command(TARGET titus POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory modules/${module_name}
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${module_target}> modules/${module_name}
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE_DIR:${module_target}>/module.json modules/${module_name}
    )
endfunction(ADD_MODULE module_name module_target)

ADD_MODULE("core-components" core_components_module)
ADD_MODULE("core-input" core_input)
ADD_MODULE("core-renderer" core_renderer)