#pragma once

#include "error/error.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct titus_plugin {
    void (*initialize)();
    void (*deinitialize)();
} titus_plugin;

typedef struct titus_plugin_array {
    titus_plugin* plugins;
    size_t count;
    size_t capacity;
} titus_plugin_array;

titus_plugin_array titus_plugin_array_new();
void titus_plugin_array_free(titus_plugin_array* arr);
titus_result titus_plugin_array_append(titus_plugin_array* arr, titus_plugin plugin);

titus_result load_plugins(titus_plugin_array* arr, const char* root);

#ifdef __cplusplus
}
#endif