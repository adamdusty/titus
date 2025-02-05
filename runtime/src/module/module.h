#pragma once

#include "ds/stb_ds.h"
#include "sds/sds.h"
#include <SDL3/SDL.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

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
    sds binary;
} titus_module_manifest;

typedef struct titus_module {
    titus_module_manifest manifest;
    SDL_SharedObject* handle;
    void (*initialize)(void*);
    void (*deinitialize)(void*);
} titus_module;

/*
    Returns an array of sds strings.
    sdsfree must be called on each string.
    arrfree must be called on the pointer, but not before the sds are freed.
*/
sds* titus_get_manifest_paths_from_root(const char* root);

sds titus_version_to_string(const titus_module_version* ver);
sds titus_module_dir_from_manifest(const titus_module_manifest* man);
bool titus_load_manifest_from_path(const char* path, titus_module_manifest* out);
bool titus_parse_manifest(char* data, size_t len, titus_module_manifest* out);
bool titus_validate_manifest(titus_module_manifest* man, char** msg);

#ifdef __cplusplus
}
#endif
