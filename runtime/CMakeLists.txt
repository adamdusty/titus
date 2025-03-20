add_library(titus_runtime_lib OBJECT)
target_compile_options(titus_runtime_lib PRIVATE ${DEBUG_COMPILER_FLAGS})
target_link_libraries(titus_runtime_lib PUBLIC titus::sdk yyjson)
target_include_directories(titus_runtime_lib PUBLIC src)
target_sources(titus_runtime_lib
    PRIVATE
        src/config/config.c
        src/module/module.c
        src/module/pack.c
        src/serialization/serialization.c
)


add_executable(titus_rt)
target_compile_options(titus_rt PRIVATE ${DEBUG_COMPILER_FLAGS})
target_link_libraries(titus_rt titus_runtime_lib)
target_sources(titus_rt PRIVATE src/main.c)