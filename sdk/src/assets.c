#include "titus/assets/assets.h"

#include "titus/assert/assert.h"
#include "titus/ds/stb_ds.h"
#include "titus/log/log.h"
#include "titus/module/module.h"
#include <SDL3/SDL.h>

/// Using the module namespace and name, looks up the module's resource path and returns
/// the path appended to the resource directory, if a file exists at that path.
/// @param ecs The ecs world.
/// @param namespace The namespace of the module.
/// @param name The name of the module.
/// @param path The path of the resource relative to the module's resource directory.
/// @return The path to the resource if it exists. NULL if nothing exists at the path or there is an error.
/// @note The `char*` returned is actually an `sds` from `titus/sds/sds.h`. It should be freed by calling `sdsfree()`
/// when no longer needed
sds titus_get_asset_path(ecs_world_t* ecs, char* namespace, char* name, char* path) {
    TITUS_ASSERT(ecs != NULL);                                       // precondition: Attempting to read from NULL
    TITUS_ASSERT(namespace != NULL && name != NULL && path != NULL); // precondition

    // ecs_entity_t module_map_component = ecs_lookup(ecs, "runtime:module_map");
    // if(module_map_component == 0) {
    //     titus_log_error("Failed to find module map component id.");
    //     return NULL;
    // }

    // TitusModuleHashmapKV* mod_map =
    //     *(TitusModuleHashmapKV**)ecs_get_mut_id(ecs, module_map_component, module_map_component);
    // if(NULL == mod_map) {
    //     titus_log_error("Failed to find module map entity");
    //     return NULL;
    // }

    // sds p = sdsempty();
    // p     = sdscatfmt(p, "%s:%s", namespace, name);

    // TitusModuleHashmapKV* mod = shgetp(mod_map, p);
    // if(NULL == mod) {
    //     titus_log_error("Failed to get module. %s not in map.", p);
    //     sdsfree(p);
    //     return NULL;
    // }
    // sdsfree(p);

    // p = sdsempty();
    // p = sdscatfmt(p, "%S%s", mod->value.resource_path, path);

    // if(!SDL_GetPathInfo(p, NULL)) {
    //     titus_log_error("No resource found at path: %s", p);
    //     sdsfree(p);
    //     return NULL;
    // }

    return NULL;
}
