#include "module/pack.h"

#include "serialization/serialization.h"
#include <SDL3/SDL.h>
#include <titus/assert.h>

static constexpr char PACK_EXT[] = "pack";

/**
 * @brief Module pack constructor. Moves `name` and `modules`. `modules` must be a malloc'd array.
 *
 * @param name
 * @param modules Malloc'd array of metadata objects
 * @param count Number of metadata objects in `modules`.
 * @return TitusModulePack
 */
TitusModulePack titus_module_pack_create(sds name, TitusRequiredModule* modules, size_t count) {
    TitusModulePack pack = {.name = sdsdup(name), .module_count = count};
    pack.modules         = malloc(sizeof(TitusRequiredModule) * count);
    if(pack.modules == NULL) {
        titus_log_error("Failed to allocate memory: %s:%d [%s]", __FILE__, __LINE__, __func__);
        titus_log_error("Exiting application.");
        exit(EXIT_FAILURE);
    }

    SDL_memcpy(pack.modules, modules, sizeof(TitusRequiredModule) * count);
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

SDL_EnumerationResult pack_json_cb(void* user_data, const char* dir, const char* file) {
    sds path = sdsnew(dir);
    path     = sdscat(path, file);

    SDL_PathInfo info = {0};
    SDL_GetPathInfo(path, &info);

    if(SDL_PATHTYPE_DIRECTORY == info.type) {
        SDL_EnumerateDirectory(path, pack_json_cb, user_data);
    }

    if(file != NULL && strstr(file, PACK_EXT) == NULL) {
        sdsfree(path);
        return SDL_ENUM_CONTINUE; // Continue search entry is not a .pack file
    }

    if(SDL_PATHTYPE_FILE == info.type) {
        TitusModulePack** packs = user_data;

        yyjson_read_err err;
        yyjson_read_flag flags = YYJSON_READ_ALLOW_COMMENTS | YYJSON_READ_ALLOW_TRAILING_COMMAS;
        yyjson_doc* doc        = yyjson_read_file(path, flags, NULL, &err);
        if(doc == NULL) {
            titus_log_error("Failed to read pack file [%s]: %s", path, err.msg);
            sdsfree(path);
            return SDL_ENUM_CONTINUE;
        }

        TitusModulePackDeserializationResult result = titus_module_pack_deserialize(yyjson_doc_get_root(doc));
        if(result.error != NULL) {
            titus_log_warn("Error encountered while deserializing pack [%s]: %s", path, result.error);
            sdsfree(path);
            return SDL_ENUM_CONTINUE;
        }

        arrpush(*packs, result.pack);

        sdsfree(path);
        return SDL_ENUM_CONTINUE;
    };

    sdsfree(path);
    return SDL_ENUM_CONTINUE;
}

/**
 * @brief Recursively searches for .json files in `root`. Returns NULL on failure, otherwise an std dynamic array of all
 * the available module packs. The returned pointer should be freed with `arrfree`.
 *
 * @param root Pack root directory.
 * @return TitusModulePack* stb dyanmic array of TitusModulePack
 */
TitusModulePack* titus_get_module_packs(sds root) {
    TitusModulePack* pack_arr    = NULL;
    SDL_EnumerationResult result = SDL_EnumerateDirectory(root, pack_json_cb, &pack_arr);
    if(result != SDL_ENUM_SUCCESS) {
        titus_log_error("Error while enumerating directories: %s", SDL_GetError());
    }

    return pack_arr;
}
