project(
    core-core
    VERSION 0.1.0
)

if(NOT TARGET titus::sdk)
find_package(titus CONFIG REQUIRED)
endif(NOT TARGET titus::sdk)

add_library(core_headers INTERFACE)
target_link_libraries(core_headers INTERFACE SDL3::SDL3 titus::sdk)
target_sources(core_headers
    INTERFACE
        FILE_SET core_export_header
        TYPE     HEADERS
        BASE_DIRS "${CMAKE_CURRENT_BINARY_DIR}/export"
        FILES
            "${CMAKE_CURRENT_BINARY_DIR}/export/core/export.h"
    INTERFACE
        FILE_SET core_component_interface_headers
        TYPE     HEADERS
        BASE_DIRS include
        FILES
            "include/core/components.h"
            "include/core/input.h"
            "include/core/rendering.h"
)

add_library(core_module MODULE)
target_link_libraries(core_module PRIVATE core_headers)
target_compile_options(core_module PRIVATE ${DEBUG_COMPILER_FLAGS})
target_sources(core_module
    PUBLIC
        FILE_SET core_export_header
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
    PRIVATE
        src/core.c
)

configure_file(module.json module.json)

include(GenerateExportHeader)
generate_export_header(
    core_module
    BASE_NAME core
    EXPORT_FILE_NAME "export/core/export.h"
    CUSTOM_CONTENT_FROM_VARIABLE pragma_suppress_c4251
)

set_target_properties(
    core_module PROPERTIES
    CXX_VISIBILITY_PRESET "hidden"
    VISIBILITY_INLINES_HIDDEN YES
    VERSION "${PROJECT_VERSION}"
    NO_SONAME 1
    # SOVERSION "1"
    # SOVERSION "${PROJECT_VERSION_MAJOR}"
    PREFIX ""
    EXPORT_NAME core
    OUTPUT_NAME core
)