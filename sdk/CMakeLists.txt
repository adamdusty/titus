add_library(titus_sdk SHARED)
add_library(titus::sdk ALIAS titus_sdk)
target_link_libraries(titus_sdk PUBLIC SDL3::SDL3 flecs::flecs cglm)
target_compile_options(titus_sdk PRIVATE ${DEBUG_COMPILER_FLAGS})
target_sources(titus_sdk
    PUBLIC
        FILE_SET sdk_export_header
        TYPE     HEADERS
        BASE_DIRS "${CMAKE_CURRENT_BINARY_DIR}/export"
        FILES
            "${CMAKE_CURRENT_BINARY_DIR}/export/titus/export.h"
    PUBLIC
        FILE_SET sdk_headers
        TYPE     HEADERS
        BASE_DIRS include
        FILES
            "include/titus/sdk.h"
            "include/titus/context/context.h"
            "include/titus/ecs/ecs.h"
            "include/titus/log/log.h"
            "include/titus/module/module.h"
            "include/titus/sds/sds.h"
            "include/titus/ds/stb_ds.h"
            "include/titus/linalg/vec.h"
            "include/titus/linalg/mat.h"
    PRIVATE
        src/sdk.c
        src/ecs.c
        src/log.c
        src/assets.c
        src/mat.c
        src/vec.c
        src/sds.c
        src/stb_ds.c
)

include(GenerateExportHeader)
generate_export_header(
    titus_sdk
    BASE_NAME titus
    EXPORT_FILE_NAME "export/titus/export.h"
    CUSTOM_CONTENT_FROM_VARIABLE pragma_suppress_c4251
)

set_target_properties(
    titus_sdk PROPERTIES
    C_VISIBILITY_PRESET "hidden"
    VISIBILITY_INLINES_HIDDEN YES
    VERSION "${PROJECT_VERSION}"
    SOVERSION "${PROJECT_VERSION_MAJOR}"
    EXPORT_NAME titus
    OUTPUT_NAME titus
)