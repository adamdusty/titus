if(NOT TARGET titus::sdk)
find_package(titus CONFIG REQUIRED)
endif(NOT TARGET titus::sdk)

add_library(core_components INTERFACE)
target_link_libraries(core_components INTERFACE SDL3::SDL3 titus::sdk)
target_sources(core_components
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
)

add_library(core_components_module SHARED)
target_link_libraries(core_components_module PRIVATE core_components)
target_sources(core_components_module
    PRIVATE
        src/components.c
)

configure_file(module.json module.json)

include(GenerateExportHeader)
generate_export_header(
    core_components_module
    BASE_NAME core
    EXPORT_FILE_NAME "export/core/export.h"
    CUSTOM_CONTENT_FROM_VARIABLE pragma_suppress_c4251
)

set_target_properties(
    core_components_module PROPERTIES
    CXX_VISIBILITY_PRESET "hidden"
    VISIBILITY_INLINES_HIDDEN YES
    VERSION "${PROJECT_VERSION}"
    SOVERSION "${PROJECT_VERSION_MAJOR}"
    EXPORT_NAME components
    OUTPUT_NAME components
)