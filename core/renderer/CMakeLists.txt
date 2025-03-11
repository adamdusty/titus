project(
    core-renderer
    VERSION 0.1.0
)

if(NOT TARGET titus::sdk)
find_package(titus CONFIG REQUIRED)
endif(NOT TARGET titus::sdk)

add_library(core_renderer MODULE)
target_link_libraries(core_renderer PRIVATE titus::sdk core_module)
target_sources(core_renderer
    PUBLIC
        FILE_SET core_renderer_export_header
        TYPE     HEADERS
        BASE_DIRS "${CMAKE_CURRENT_BINARY_DIR}/export"
        FILES
            "${CMAKE_CURRENT_BINARY_DIR}/export/renderer/export.h"
    PRIVATE
        FILE_SET core_renderer_headers
        TYPE     HEADERS
        BASE_DIRS src
        FILES
            src/shaders.h
            src/gpu.h
            src/context.h
            src/components.h
            src/phong/pipeline.h
    PRIVATE
        src/renderer.c
        src/shaders.c
        src/gpu.c
        src/phong/pipeline.c
)

configure_file(module.json module.json)

include(GenerateExportHeader)
generate_export_header(
    core_renderer
    BASE_NAME core_renderer
    EXPORT_FILE_NAME "export/renderer/export.h"
    CUSTOM_CONTENT_FROM_VARIABLE pragma_suppress_c4251
)

set_target_properties(
    core_renderer PROPERTIES
    C_VISIBILITY_PRESET "hidden"
    VISIBILITY_INLINES_HIDDEN YES
    VERSION "${PROJECT_VERSION}"
    SOVERSION "${PROJECT_VERSION_MAJOR}"
    PREFIX ""
    EXPORT_NAME renderer
    OUTPUT_NAME renderer
)

add_custom_target(resources ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different ${CMAKE_CURRENT_SOURCE_DIR}/resources ${CMAKE_CURRENT_BINARY_DIR}/resources
)

# LLM GENERATED 
set(SHADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders")
set(SHADER_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/resources/shaders")
file(MAKE_DIRECTORY ${SHADER_BUILD_DIR})

# Function to compile a shader
function(compile_shader SHADER_PATH OUTPUT_VARIABLE SHADER_TYPE)
  execute_process(
    COMMAND glslc "${SHADER_PATH}" -o "${SHADER_BUILD_DIR}/${OUTPUT_VARIABLE}.spv" -fshader-stage=${SHADER_TYPE}
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE error
  )

  if(NOT result EQUAL 0)
    message(FATAL_ERROR "Failed to compile shader: ${SHADER_PATH}\nOutput: ${output}\nError: ${error}")
  endif()

  set(${OUTPUT_VARIABLE} "${SHADER_BUILD_DIR}/${OUTPUT_VARIABLE}.spv" PARENT_SCOPE)
endfunction()

# Find all shader files
file(GLOB SHADER_FILES "${SHADER_DIR}/*.vert" "${SHADER_DIR}/*.frag")

# Compile each shader
foreach(SHADER_FILE ${SHADER_FILES})
  get_filename_component(SHADER_NAME ${SHADER_FILE} NAME_WE)
  get_filename_component(SHADER_EXT ${SHADER_FILE} EXT)

  if(SHADER_EXT STREQUAL ".vert")
    compile_shader("${SHADER_FILE}" "${SHADER_NAME}.vert" vertex)
    list(APPEND VERTEX_SHADERS "${SHADER_NAME}.vert")
  elseif(SHADER_EXT STREQUAL ".frag")
    compile_shader("${SHADER_FILE}" "${SHADER_NAME}.frag" fragment)
    list(APPEND FRAGMENT_SHADERS "${SHADER_NAME}.frag")
  endif()
endforeach()