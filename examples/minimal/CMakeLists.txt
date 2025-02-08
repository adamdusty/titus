add_library(minimal SHARED src/mod.c)
target_link_libraries(minimal PRIVATE titus::sdk)