project(
    core-core
    VERSION 0.1.0
)

if(NOT TARGET titus::sdk)
find_package(titus CONFIG REQUIRED)
endif(NOT TARGET titus::sdk)

add_library(core_module SHARED)
target_link_libraries(core_module PRIVATE titus::sdk)
target_compile_options(core_module PRIVATE ${DEBUG_COMPILER_FLAGS})
target_sources(core_module
    PUBLIC
        FILE_SET core_module_export_header
        TYPE     HEADERS
        BASE_DIRS "${CMAKE_CURRENT_BINARY_DIR}/export"
        FILES
            "${CMAKE_CURRENT_BINARY_DIR}/export/core/export.h"
    PUBLIC
        FILE_SET core_module_headers
        TYPE HEADERS
        BASE_DIRS include
        FILES
            "include/core/core.h"
            "include/core/input.h"
            "include/core/rendering.h"
    PRIVATE
        src/core.c
)

configure_file(module.json module.json)

include(GenerateExportHeader)
generate_export_header(
    core_module
    BASE_NAME core_module
    EXPORT_FILE_NAME "export/core/export.h"
    CUSTOM_CONTENT_FROM_VARIABLE pragma_suppress_c4251
)

set_target_properties(
    core_module PROPERTIES
    C_VISIBILITY_PRESET "hidden"
    VISIBILITY_INLINES_HIDDEN YES
    PREFIX ""
    EXPORT_NAME core
    OUTPUT_NAME core
)
