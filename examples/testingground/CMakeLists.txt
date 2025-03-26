project(
    testinggrounds
    VERSION 0.1.0
)

add_library(testingground MODULE)
target_link_libraries(testingground PRIVATE titus::sdk core_module)
target_sources(testingground
    PUBLIC
        FILE_SET testingground_export_header
        TYPE     HEADERS
        BASE_DIRS "${CMAKE_CURRENT_BINARY_DIR}/export"
        FILES
            "${CMAKE_CURRENT_BINARY_DIR}/export/testing/export.h"
    PRIVATE
        src/testing.c
)

configure_file(module.json module.json)

include(GenerateExportHeader)
generate_export_header(
    testingground
    BASE_NAME tg
    EXPORT_FILE_NAME "export/testing/export.h"
    CUSTOM_CONTENT_FROM_VARIABLE pragma_suppress_c4251
)

set_target_properties(
    testingground PROPERTIES
    C_VISIBILITY_PRESET "hidden"
    VISIBILITY_INLINES_HIDDEN YES
    VERSION "${PROJECT_VERSION}"
    SOVERSION "${PROJECT_VERSION_MAJOR}"
    PREFIX ""
    EXPORT_NAME testing
    OUTPUT_NAME testing
)