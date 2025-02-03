add_executable(titus_rt)
target_link_libraries(titus_rt SDL3::SDL3 flecs::flecs)
target_include_directories(titus_rt PRIVATE src)
target_sources(titus_rt
    PRIVATE
        src/main.c
        src/config/config.c
        src/plugin/plugin.c
        src/error/error.c
        src/sds/sds.c
)