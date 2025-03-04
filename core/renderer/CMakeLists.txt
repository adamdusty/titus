project(
    core-renderer
    VERSION 0.1.0
)

if(NOT TARGET titus::sdk)
find_package(titus CONFIG REQUIRED)
endif(NOT TARGET titus::sdk)

add_library(core_renderer MODULE)
target_link_libraries(core_renderer PRIVATE titus::sdk core_headers)# core_module)
target_sources(core_renderer
    PUBLIC
        FILE_SET core_export_header
        TYPE     HEADERS
        BASE_DIRS "${CMAKE_CURRENT_BINARY_DIR}/export"
        FILES
            "${CMAKE_CURRENT_BINARY_DIR}/export/core/export.h"
    PRIVATE
        FILE_SET core_renderer_headers
        TYPE     HEADERS
        BASE_DIRS src
        FILES
            src/shaders.h
            src/gpu.h
            src/context.h
            src/components.h
    PRIVATE
        src/renderer.c
        src/shaders.c
        src/gpu.c
)

configure_file(module.json module.json)

include(GenerateExportHeader)
generate_export_header(
    core_renderer
    BASE_NAME core
    EXPORT_FILE_NAME "export/core/export.h"
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

add_custom_command(TARGET core_renderer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different ${CMAKE_CURRENT_SOURCE_DIR}/resources ${CMAKE_CURRENT_BINARY_DIR}/resources
)