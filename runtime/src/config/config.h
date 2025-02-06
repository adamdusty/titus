#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct titus_config {
    const char* log_level;
    const char* module_root_directory;
} titus_config;

titus_config titus_load_config(const char* path);
titus_config titus_default_config();

#ifdef __cplusplus
}
#endif
