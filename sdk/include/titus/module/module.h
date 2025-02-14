#pragma once

#include "titus/context/context.h"
#include "titus/sds/sds.h"
#include <SDL3/SDL.h>
#include <stddef.h>
#include <stdint.h>

typedef struct titus_module_version {
    uint64_t major;
    uint64_t minor;
    uint64_t patch;
    sds annotation;
} titus_module_version;

typedef struct titus_module_manifest {
    sds namespace;
    sds name;
    titus_module_version version;
    sds binary; // Filename of binary without extension
} titus_module_manifest;

typedef struct titus_module_load_info {
    titus_module_manifest manifest;
    sds binary;    // Relative path to binary
    sds resources; // Relative path to resources directory
    sds base;      // Module base directory
} titus_module_load_info;

typedef void (*titus_initialize_proc)(const titus_application_context*);
typedef void (*titus_deinitialize_proc)(const titus_application_context*);

typedef struct titus_module {
    titus_module_manifest manifest;
    SDL_SharedObject* handle;
    sds directory_path;
    sds binary_path;
    sds resource_path;

    titus_initialize_proc initialize;
    titus_deinitialize_proc deinitialize;
} titus_module;

typedef struct module_kv {
    char* key;
    titus_module value;
} module_kv;
