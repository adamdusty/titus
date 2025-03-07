#include "module.h"

#include "assert/assert.h"
#include <SDL3/SDL.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN

titus_module titus_load_module(titus_module_load_info* load_info) {
    TITUS_ASSERT(load_info != NULL); // precondition: NULL load_info

    titus_module module   = {0};
    module.manifest       = load_info->manifest;
    module.directory_path = load_info->base;
    titus_log_info("module directory path: %s", module.directory_path);

    titus_log_info("Attempting to load binary from: %s", load_info->binary);
    if(SDL_GetPathInfo(load_info->binary, NULL)) {
        module.binary_path = load_info->binary;
        module.handle      = (SDL_SharedObject*)LoadLibrary(load_info->binary);

        if(NULL == module.handle) {
            titus_log_error("Failed to load object");
        } else {
            titus_log_info("Loaded module binary at: %s", load_info->binary);
        }
    }

    if(SDL_GetPathInfo(load_info->resources, NULL)) {
        module.resource_path = load_info->resources;
    }

    if(NULL != module.handle) {
        module.initialize   = (titus_initialize_proc)SDL_LoadFunction(module.handle, TITUS_MODULE_INITIALIZE);
        module.deinitialize = (titus_deinitialize_proc)SDL_LoadFunction(module.handle, TITUS_MODULE_DEINITIALIZE);
    }

    load_info->manifest  = (titus_module_manifest){0};
    load_info->binary    = NULL;
    load_info->resources = NULL;

    return module;
}
