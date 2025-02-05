add_library(titus_lib OBJECT)
target_compile_options(titus_lib PRIVATE ${DEBUG_COMPILER_FLAGS})
target_link_libraries(titus_lib PUBLIC SDL3::SDL3 flecs::flecs yyjson)
target_include_directories(titus_lib PUBLIC src)
target_sources(titus_lib
    PRIVATE
        src/config/config.c
        src/module/module.c
        src/error/error.c
        src/sds/sds.c
        src/ds/stb_ds.c
)


add_executable(titus_rt)
target_compile_options(titus_rt PRIVATE ${DEBUG_COMPILER_FLAGS})
target_link_libraries(titus_rt titus_lib)
target_sources(titus_rt PRIVATE src/main.c)