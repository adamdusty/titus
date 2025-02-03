#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct titus_config {
    const char* test;
} titus_config;

titus_config titus_load_config(const char* path);

#ifdef __cplusplus
}
#endif