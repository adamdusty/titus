cmake_minimum_required(VERSION 3.29)

project(
    titus-sdk
    VERSION 0.1.0
    DESCRIPTION "SDK for titus application runtime"
    LANGUAGES CXX
)

include(cmake/dependencies.cmake)

add_library(titus_sdk SHARED)
add_library(titus::sdk ALIAS titus_sdk)

target_link_libraries(titus_sdk PUBLIC flecs::flecs quill::quill)

target_sources(titus_sdk
    PUBLIC
        FILE_SET sdk_export_header
        TYPE     HEADERS
        BASE_DIRS "${CMAKE_CURRENT_BINARY_DIR}/export"
        FILES
            "${CMAKE_CURRENT_BINARY_DIR}/export/titus/export.hpp"
    PUBLIC
        FILE_SET sdk_headers
        TYPE     HEADERS
        BASE_DIRS include
        FILES
            "include/titus/sdk.hpp"
    PRIVATE
        src/sdk.cpp
)


include(GenerateExportHeader)
generate_export_header(
    titus_sdk
    BASE_NAME titus
    EXPORT_FILE_NAME "export/titus/export.hpp"
    CUSTOM_CONTENT_FROM_VARIABLE pragma_suppress_c4251
)

set_target_properties(
    titus_sdk PROPERTIES
    CXX_VISIBILITY_PRESET "hidden"
    VISIBILITY_INLINES_HIDDEN YES
    VERSION "${PROJECT_VERSION}"
    SOVERSION "${PROJECT_VERSION_MAJOR}"
    EXPORT_NAME titus
    OUTPUT_NAME titus
)

target_compile_options(titus_sdk PRIVATE
    $<$<CONFIG:Release>:-U_FORTIFY_SOURCE>
    $<$<CONFIG:Release>:-D_FORTIFY_SOURCE=3>
    -fno-omit-frame-pointer
    -fstack-protector-strong
    -fcf-protection=full
    -Wall
    -Wextra
    -Wpedantic
    -Wconversion
    -Wsign-conversion
    -Wcast-qual
    -Wformat=2
    -Wundef
    -Werror=float-equal
    -Wcast-align
    -Wunused
    -Wnull-dereference
    -Wdouble-promotion
    -Wimplicit-fallthrough
    -Wextra-semi
    -Woverloaded-virtual
    -Wnon-virtual-dtor
    -Wold-style-cast
)