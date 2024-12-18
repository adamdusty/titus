cmake_minimum_required(VERSION 3.29)

project(
    titus
    LANGUAGES CXX
)

if("${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}")
   message(SEND_ERROR "In-source builds are not allowed.")
endif()

add_subdirectory(sdk)
add_subdirectory(runtime)

if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()