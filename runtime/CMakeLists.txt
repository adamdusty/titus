add_executable(titus_rt)
target_sources(titus_rt
    PRIVATE
        src/main.cpp
)

target_link_libraries(titus_rt PRIVATE titus::sdk)

set_target_properties(
    titus_rt PROPERTIES
    OUTPUT_NAME titus
)

add_custom_command(TARGET titus_rt POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    $<TARGET_RUNTIME_DLLS:titus_rt>
    $<TARGET_FILE_DIR:titus_rt>
    COMMAND_EXPAND_LISTS
)

target_compile_options(titus_rt PRIVATE
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