cmake_minimum_required(VERSION 3.31)

project(titus LANGUAGES C)

option(TITUS_FETCHCONTENT "Download dependencies via Cmake FetchContent" ON)
option(TITUS_TESTING "Build tests for titus" OFF)

add_compile_definitions(CGLM_FORCE_DEPTH_ZERO_TO_ONE)

include(cmake/compile.cmake)
include(cmake/dependencies.cmake)
add_subdirectory(sdk)
add_subdirectory(runtime)
add_subdirectory(core)

add_subdirectory(examples)
add_subdirectory(package)

if(TITUS_TESTING)
enable_testing()
add_subdirectory(tests)
endif(TITUS_TESTING)