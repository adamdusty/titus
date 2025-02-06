#include <stdio.h>

#if defined(_WIN32)
#define PLUGIN_EXPORT __declspec(dllexport)
#elif defined(__linux__)
#define PLUGIN_EXPORT __attribute__((visibility("default")))
#endif

PLUGIN_EXPORT void titus_initialize(void* ud) {
    printf("Hello from minimal plugin initialization\n");
}

PLUGIN_EXPORT void titus_deinitialize(void* ud) {
    printf("Hello from minimal plugin deinitialization\n");
}