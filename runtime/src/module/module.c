// TODO: Add directory path to module on load

#include "module/module.h"

#include "titus/ds/stb_ds.h"
#include "titus/sds/sds.h"
#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <titus/assert/assert.h>
#include <yyjson.h>

#ifdef __linux__
#define SHARED_OBJECT_FILE_EXT "so"
#elif defined(_WIN32)
#define SHARED_OBJECT_FILE_EXT "dll"
#endif

static const char* MANIFEST_FILE_NAME = "module.json";

bool titus_parse_manifest(char* data, size_t len, TitusModuleMetaData* out);

SDL_EnumerationResult manifest_cb(void* user_data, const char* dir, const char* file) {
    sds path = sdsnew(dir);
    path     = sdscat(path, file);

    SDL_PathInfo info = {0};
    SDL_GetPathInfo(path, &info);

    if(SDL_PATHTYPE_FILE == info.type && SDL_strcmp(MANIFEST_FILE_NAME, file) == 0) {
        ecs_world_t* ecs = user_data;

        TitusModuleMetaData meta = {0};
        if(!titus_deserialize_manifest(path, &meta)) {
            titus_log_error("Failed to deserialize manifest at path: %s", path);

            sdsfree(path);
            return SDL_ENUM_CONTINUE;
        }

        sds entity_name     = sdsdup(meta.namespace);
        entity_name         = sdscatfmt(entity_name, ":%S", meta.name);
        ecs_entity_t module = ecs_entity(ecs, {.name = entity_name});
        titus_log_info("Module entity: %d", module);

        TitusModuleMetaData* module_meta      = ecs_ensure(ecs, module, TitusModuleMetaData);
        TitusModulePathInfo* module_path_info = ecs_ensure(ecs, module, TitusModulePathInfo);

        module_meta->namespace        = meta.namespace;
        module_meta->name             = sdsnew("name");
        module_meta->binary_file_name = sdsnew("bin");
        module_meta->version          = (TitusVersion){.major = 1};
        titus_log_info("Module ready to load: %s:%s", module_meta->namespace, module_meta->name);

        module_path_info->root_directory = sdsnew(dir);

        module_path_info->binary_path = sdsdup(module_path_info->root_directory);
        module_path_info->binary_path =
            sdscatfmt(module_path_info->binary_path, "%S.%s", meta.binary_file_name, SHARED_OBJECT_FILE_EXT);

        module_path_info->resource_directory = sdsdup(module_path_info->root_directory);
        module_path_info->resource_directory = sdscat(module_path_info->resource_directory, "/resources");
        if(!SDL_GetPathInfo(module_path_info->resource_directory, NULL)) {
            sdsfree(module_path_info->resource_directory);
            module_path_info->resource_directory = NULL;
        }

        sdsfree(path);
        return SDL_ENUM_CONTINUE;
    };

    if(SDL_PATHTYPE_DIRECTORY == info.type) {
        SDL_EnumerateDirectory(path, manifest_cb, user_data);
    }

    sdsfree(path);
    return SDL_ENUM_CONTINUE;
}

// Get available modules and return a stb dynamic array
void titus_get_available_modules(ecs_world_t* ecs, const char* root) {
    bool result = SDL_EnumerateDirectory(root, manifest_cb, ecs);
    if(!result) {
        titus_log_error("Failed to enumerate module directory: %s", SDL_GetError());
    }
}

// Load modules from a stb dynamic array of module metadata
void titus_load_modules(ecs_world_t* ecs, TitusModuleMetaData* module_metadatas) {
    abort();
}

// Unloads module and frees metadata
void titus_free_module(TitusModule* mod) {
    sdsfree(mod->metadata.namespace);
    sdsfree(mod->metadata.name);
    sdsfree(mod->metadata.binary_file_name);

    SDL_UnloadObject(mod->binary);

    mod->metadata     = (TitusModuleMetaData){0};
    mod->binary       = NULL;
    mod->initialize   = NULL;
    mod->deinitialize = NULL;
}

// sds titus_module_dir_from_manifest(const TitusModuleManifest* man) {
//     TITUS_ASSERT(NULL != man);            // precondition: Attempting to read from NULL
//     TITUS_ASSERT(NULL != man->namespace); // precondition: Attempting to read from NULL
//     TITUS_ASSERT(NULL != man->name);      // precondition: Attempting to read from NULL

//     sds path = sdsempty();
//     path     = sdscatfmt(path, "%s/%s/", man->namespace, man->name);
//     sds ver  = titus_version_to_string(&man->version);
//     path     = sdscatsds(path, ver);
//     sdsfree(ver);

//     return path;
// }

// SDL_EnumerationResult manifest_callback(void* ud, const char* d, const char* f) {
//     TITUS_ASSERT(NULL != ud); // precondition: Attempting to write to NULL
//     titus_log_verbose("Enumerating directory %s: %s", d, f);

//     if(SDL_strcmp(f, MANIFEST_FILE_NAME) != 0) {
//         return SDL_ENUM_CONTINUE;
//     }

//     sds path = sdsnew(d);
//     path     = sdscat(path, f);

//     TitusModuleManifest man = {0};
//     if(!titus_load_manifest_from_path(path, &man)) {
//         titus_log_error("Found module.json (%s), but failed to load", path);
//         return SDL_ENUM_CONTINUE;
//     }
//     TitusModuleLoadInfo li = {.manifest = man, .binary = NULL, .resources = NULL, .base = sdsnew(d)};

//     if(li.manifest.binary) {
//         li.binary = sdsnew(d);
//         li.binary = sdscatfmt(li.binary, "%S.%s", man.binary, SHARED_OBJECT_FILE_EXT);
//     }

//     sds res = sdsnew(d);
//     res     = sdscat(res, "resources/");
//     if(SDL_GetPathInfo(res, NULL)) {
//         titus_log_debug("Resource folder found for %s:%s", li.manifest.namespace, li.manifest.name);
//         li.resources = res;
//     } else {
//         titus_log_debug("No resource folder found for module: %s:%s", li.manifest.namespace, li.manifest.name);
//         titus_log_debug("Searched for resource folder at: %s", res);
//         sdsfree(res);
//     }

//     TitusModuleLoadInfo** li_arr = ud;
//     arrpush(*li_arr, li);

//     titus_log_debug("Successfully gathered load info from %s", d);

//     return SDL_ENUM_CONTINUE;
// }

// SDL_EnumerationResult modules_callback(void* ud, const char* d, const char* f) {
//     // Enumerate over directories in the namespace level of the folder
//     // Callback for each module name in this level
//     titus_log_verbose("Enumerating directory %s: %s", d, f);

//     sds path = sdsnew(d);
//     path     = sdscat(path, f);

//     if(NULL == path) {
//         return SDL_ENUM_FAILURE;
//     }
//     SDL_PathInfo info = {0};
//     SDL_GetPathInfo(path, &info);

//     if(SDL_PATHTYPE_DIRECTORY == info.type) {
//         SDL_EnumerateDirectory(path, manifest_callback, ud);
//     }

//     sdsfree(path);
//     return SDL_ENUM_CONTINUE;
// }

bool titus_deserialize_manifest(sds path, TitusModuleMetaData* out) {
    TITUS_ASSERT(path != NULL) // precondition: Attempting to dereference NULL
    TITUS_ASSERT(out != NULL)  // precondition: Attempting to assign to NULL

    if(!SDL_GetPathInfo(path, NULL)) {
        titus_log_error("%s", SDL_GetError());
        return false;
    }

    size_t num_bytes = 0;
    char* bytes      = SDL_LoadFile(path, &num_bytes);
    if(NULL == bytes) {
        titus_log_error("Failed to read manifest: %s", SDL_GetError());
        return false;
    }

    if(!titus_parse_manifest(bytes, num_bytes, out)) {
        titus_log_error("Failed to parse manifest at: %s", path);
    }

    SDL_free(bytes);
    return true;
}

// sds* titus_get_manifest_paths_from_root(const char* root) {
//     TITUS_ASSERT(NULL != root); // precondition: Attempting to read from NULL

//     sds* paths  = NULL;
//     bool result = SDL_EnumerateDirectory(root, modules_callback, &paths);
//     if(!result) {
//         titus_log_error("Error while enumerating directory: %s", SDL_GetError());
//     }

//     return paths;
// }

// bool titus_load_manifest_from_path(const char* path, TitusModuleManifest* out) {
//     TITUS_ASSERT(path != NULL); // precondition: Attempting to load from empty path
//     TITUS_ASSERT(out != NULL);  // precondition: Attempting to write to NULL

//     // Check if path exists
//     if(!SDL_GetPathInfo(path, NULL)) {
//         titus_log_warn("Manifest file does not exists at path: %s", path);
//         return false;
//     }

//     size_t data_size = 0;
//     char* data       = SDL_LoadFile(path, &data_size);

//     return titus_parse_manifest(data, data_size, out);
// }

// Deserialize a module manifest file to get it's metadata
bool titus_parse_manifest(char* data, size_t len, TitusModuleMetaData* out) {
    TITUS_ASSERT(data != NULL); // precondition: Attempting to read from NULL
    TITUS_ASSERT(out != NULL);  // precondition: Attempting to write to NULL
    TITUS_ASSERT(len != 0);     // precondition: Zero-length byte array for data

    bool namespace_found = false;
    bool name_found      = false;
    bool binary_found    = false;
    bool version_found   = false;

    yyjson_read_err err;
    yyjson_read_flag flags = YYJSON_READ_ALLOW_COMMENTS | YYJSON_READ_ALLOW_TRAILING_COMMAS;
    yyjson_doc* doc        = yyjson_read_opts(data, len, flags, NULL, &err);

    if(NULL == doc) {
        titus_log_error("Failed to read json document");

        size_t line = 0;
        size_t col  = 0;
        size_t ch   = 0;
        if(yyjson_locate_pos(data, len, err.pos, &line, &col, &ch)) {
            titus_log_error("%s: [%zu: %zu]", err.msg, line, col);
        }
        return false;
    }

    yyjson_val* root = yyjson_doc_get_root(doc);

    // manifest root should be an object
    if(yyjson_get_type(root) != YYJSON_TYPE_OBJ) {
        yyjson_doc_free(doc);
        titus_log_error("module.json root should be an object");
        return false;
    }

    yyjson_val* key    = NULL;
    yyjson_val* val    = NULL;
    yyjson_obj_iter it = yyjson_obj_iter_with(root);
    while((key = yyjson_obj_iter_next(&it))) {
        val = yyjson_obj_iter_get_val(key);
        if(yyjson_equals_str(key, "namespace")) {
            out->namespace  = sdsnew(yyjson_get_str(val));
            namespace_found = true;
        }

        if(yyjson_equals_str(key, "name")) {
            out->name  = sdsnew(yyjson_get_str(val));
            name_found = true;
        }

        if(yyjson_equals_str(key, "binary")) {
            out->binary_file_name = sdsnew(yyjson_get_str(val));
            binary_found          = true;
        }

        if(yyjson_equals_str(key, "version")) {
            out->version.major      = yyjson_get_uint(yyjson_obj_get(val, "major"));
            out->version.minor      = yyjson_get_uint(yyjson_obj_get(val, "minor"));
            out->version.patch      = yyjson_get_uint(yyjson_obj_get(val, "patch"));
            out->version.annotation = sdsnew(yyjson_get_str(yyjson_obj_get(val, "annotation")));
            version_found           = true;
        }
    }

    yyjson_doc_free(doc);

    if(!namespace_found) {
        titus_log_error("module.json does not define required field: 'namespace'");
        return false;
    }

    if(!name_found) {
        titus_log_error("module.json does not define required field: 'name'");
        return false;
    }

    if(!binary_found) {
        titus_log_error("module.json does not define required field: 'binary'");
        return false;
    }

    if(!version_found) {
        titus_log_error("module.json does not define required field: 'version'");
        return false;
    }

    return true;
}

// TitusModuleLoadInfo* titus_get_module_load_info_from_dir(const char* root) {
//     TITUS_ASSERT(NULL != root); // precondition: Attempting to read from NULL

//     TitusModuleLoadInfo* loads = NULL;

//     bool result = SDL_EnumerateDirectory(root, modules_callback, &loads);
//     if(!result) {
//         titus_log_error("Error while enumerating directory: %s", SDL_GetError());
//     }

//     return loads;
// }
