#pragma once

#include "titus/sds/sds.h"
#include <SDL3/SDL.h>
#include <stdint.h>
#include <titus/module/module.h>
#include <titus/sdk.h>

#define TITUS_MODULE_INITIALIZE "titus_initialize"
#define TITUS_MODULE_DEINITIALIZE "titus_deinitialize"

/*
    Checks each directory inside `root` for a module.json file.
    If present, parses the manifest to get paths for a binary.
    Checks for a subdirectory named `resources` to add to resources.
*/
TitusModuleLoadInfo* titus_get_module_load_info_from_dir(const char* root);

/*
    If `binary` is not NULL, and a file exists at the path, attempts to load a handle to a shared library at that
   location.
   If `resources` is not NULL, and a directory exists at the path, adds the directory to the resource registry.
   Everything from `load_info` is moved into the returned TitusModule. Fields are zeroed on return.
*/
TitusModule titus_load_module(TitusModuleLoadInfo* load_info);

void titus_free_module(TitusModule* mod);

bool titus_load_manifest_from_path(const char* path, TitusModuleManifest* out);
bool titus_parse_manifest(char* data, size_t len, TitusModuleManifest* out);
bool titus_validate_manifest(TitusModuleManifest* man, char** msg);
