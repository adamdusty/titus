#include "module/pack.h"

#include <SDL3/SDL.h>
#include <titus/assert/assert.h>

/**
 * @brief Module pack constructor. Moves `name` and `modules`. `modules` must be a malloc'd array.
 *
 * @param name
 * @param modules Malloc'd array of metadata objects
 * @param count Number of metadata objects in `modules`.
 * @return TitusModulePack
 */
TitusModulePack titus_module_pack_create(sds name, TitusModuleMetaData* modules, size_t count) {
    TitusModulePack pack = {.name = sdsdup(name), .module_count = count};
    pack.modules         = malloc(sizeof(TitusModuleMetaData) * count);
    if(pack.modules == NULL) {
        titus_log_error("Failed to allocate memory: %s:%d [%s]", __FILE__, __LINE__, __FUNCTION__);
        titus_log_error("Exiting application.");
        exit(EXIT_FAILURE);
    }

    SDL_memcpy(pack.modules, modules, sizeof(TitusModuleMetaData) * count);
    return pack;
}

/**
 * @brief Frees pack.name via sdsfree and pack.modules via free. Sets all fields to 0.
 *
 * @param pack
 */
void titus_module_pack_destroy(TitusModulePack* pack) {
    sdsfree(pack->name);
    free(pack->modules);
}

/**
 * @brief Recursively searches for .json files in `root`. Returns NULL on failure, otherwise an std dynamic array of all
 * the available module packs. The returned pointer should be freed with `arrfree`.
 *
 * @param root Pack root directory.
 * @return TitusModulePack* stb dyanmic array of TitusModulePack
 */
TitusModulePack* titus_get_module_packs(sds root) {
    return NULL; // Unimplemented
}
