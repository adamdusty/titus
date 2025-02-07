if(NOT TARGET titus::sdk)
find_package(titus CONFIG REQUIRED)
endif(NOT TARGET titus::sdk)

add_library(core_input SHARED)
target_link_libraries(core_input PRIVATE titus::sdk)
target_sources(core_input
    PUBLIC
        FILE_SET sdk_export_header
        TYPE     HEADERS
        BASE_DIRS "${CMAKE_CURRENT_BINARY_DIR}/export"
        FILES
            "${CMAKE_CURRENT_BINARY_DIR}/export/input/export.h"
    PRIVATE src/mod.c
)

include(GenerateExportHeader)
generate_export_header(
    core_input
    BASE_NAME input
    EXPORT_FILE_NAME "export/input/export.h"
    CUSTOM_CONTENT_FROM_VARIABLE pragma_suppress_c4251
)

set_target_properties(
    core_input PROPERTIES
    CXX_VISIBILITY_PRESET "hidden"
    VISIBILITY_INLINES_HIDDEN YES
    VERSION "${PROJECT_VERSION}"
    SOVERSION "${PROJECT_VERSION_MAJOR}"
    EXPORT_NAME input
    OUTPUT_NAME input
)