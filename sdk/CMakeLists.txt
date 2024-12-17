cmake_minimum_required(VERSION 3.29)

project(
    titus-sdk
    VERSION 0.1.0
    DESCRIPTION "SDK for titus application runtime"
    LANGUAGES CXX
)

add_library(titus_sdk SHARED)
add_library(titus::sdk ALIAS titus_sdk)

include(cmake/compile-options.cmake)

target_sources(titus_sdk
    PUBLIC
        FILE_SET sdk_private_headers
        TYPE     HEADERS
        BASE_DIRS include
        FILES
            "include/titus/export.hpp"
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
    EXPORT_FILE_NAME include/titus/export.hpp
    CUSTOM_CONTENT_FROM_VARIABLE pragma_suppress_c4251
)

# Set properties
set_target_properties(
    titus_sdk PROPERTIES
    CXX_VISIBILITY_PRESET "hidden"
    VISIBILITY_INLINES_HIDDEN YES
    VERSION "${PROJECT_VERSION}"
    SOVERSION "${PROJECT_VERSION_MAJOR}"
    EXPORT_NAME titus
    OUTPUT_NAME titus
)