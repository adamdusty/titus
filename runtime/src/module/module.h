#pragma once

#include "sds/sds.h"
#include <SDL3/SDL.h>
#include <stdint.h>
#include <titus/sdk.h>

#define TITUS_MODULE_INITIALIZE "titus_initialize"
#define TITUS_MODULE_DEINITIALIZE "titus_deinitialize"

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
    sds binary; // Filename of binary without extension
} titus_module_manifest;

typedef struct titus_module_load_info {
    titus_module_manifest manifest;
    sds binary;    // Relative path to binary
    sds resources; // Relative path to resources directory
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

/*
    Checks each directory inside `root` for a module.json file.
    If present, parses the manifest to get paths for a binary.
    Checks for a subdirectory named `resources` to add to resources.
*/
titus_module_load_info* titus_get_module_load_info_from_dir(const char* root);

/*
    If `binary` is not NULL, and a file exists at the path, attempts to load a handle to a shared library at that
   location.
   If `resources` is not NULL, and a directory exists at the path, adds the directory to the resource registry.
   Everything from `load_info` is moved into the returned titus_module. Fields are zeroed on return.
*/
titus_module titus_load_module(titus_module_load_info* load_info);

void titus_free_module(titus_module* mod);

bool titus_load_manifest_from_path(const char* path, titus_module_manifest* out);
bool titus_parse_manifest(char* data, size_t len, titus_module_manifest* out);
bool titus_validate_manifest(titus_module_manifest* man, char** msg);

#ifdef __cplusplus
}
#endif
