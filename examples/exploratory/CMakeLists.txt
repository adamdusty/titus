project(
    exploratory
    VERSION 0.1.0
)

add_library(exploratory MODULE)
target_link_libraries(exploratory PRIVATE titus::sdk core_module)
target_sources(exploratory
    PUBLIC
        FILE_SET exploratory_export_header
        TYPE     HEADERS
        BASE_DIRS "${CMAKE_CURRENT_BINARY_DIR}/export"
        FILES
            "${CMAKE_CURRENT_BINARY_DIR}/export/exploratory/export.h"
    PRIVATE
        src/exploratory.c
)

configure_file(module.json module.json)

include(GenerateExportHeader)
generate_export_header(
    exploratory
    BASE_NAME exploratory
    EXPORT_FILE_NAME "export/exploratory/export.h"
    CUSTOM_CONTENT_FROM_VARIABLE pragma_suppress_c4251
)

set_target_properties(
    exploratory PROPERTIES
    C_VISIBILITY_PRESET "hidden"
    VISIBILITY_INLINES_HIDDEN YES
    VERSION "${PROJECT_VERSION}"
    SOVERSION "${PROJECT_VERSION_MAJOR}"
    PREFIX ""
    EXPORT_NAME exploratory
    OUTPUT_NAME exploratory
)