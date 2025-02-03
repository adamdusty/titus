cmake_minimum_required(VERSION 3.31)

project(titus LANGUAGES C)

option(TITUS_FETCHCONTENT "Download dependencies via Cmake FetchContent" ON)

include(cmake/dependencies.cmake)
add_subdirectory(runtime)