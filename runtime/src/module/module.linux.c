#include "module.h"

#include <SDL3/SDL.h>
#include <dlfcn.h>
#include <titus/assert/assert.h>

TitusModule titus_load_module(TitusModuleLoadInfo* load_info) {
    TITUS_ASSERT(load_info != NULL); // precondition: NULL load_info

    TitusModule module    = {0};
    module.manifest       = load_info->manifest;
    module.directory_path = load_info->base;
    titus_log_info("module directory path: %s", module.directory_path);

    titus_log_info("Attempting to load binary from: %s", load_info->binary);
    if(SDL_GetPathInfo(load_info->binary, NULL)) {
        module.binary_path = load_info->binary;

        dlerror(); // Clear dlerror
        module.handle = (SDL_SharedObject*)dlopen(load_info->binary, RTLD_NOW | RTLD_LOCAL);

        if(NULL == module.handle) {
            titus_log_error("Failed to load object: %s", dlerror());
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

    load_info->manifest  = (TitusModuleManifest){0};
    load_info->binary    = NULL;
    load_info->resources = NULL;

    return module;
}
