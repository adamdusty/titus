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
        TitusModule** modules = user_data;

        TitusModuleMetaData* meta = malloc(sizeof(TitusModuleMetaData));
        if(!titus_deserialize_manifest(path, meta)) {
            titus_log_error("Failed to deserialize manifest at path: %s", path);

            sdsfree(path);
            return SDL_ENUM_CONTINUE;
        }

        TitusModule mod = {.root_directory = sdsnew(dir), .metadata = meta};
        arrpush(*modules, mod);

        return SDL_ENUM_CONTINUE;
    };

    if(SDL_PATHTYPE_DIRECTORY == info.type) {
        SDL_EnumerateDirectory(path, manifest_cb, user_data);
    }

    sdsfree(path);
    return SDL_ENUM_CONTINUE;
}

// TODO: document
TitusModule* titus_get_available_modules(sds root) {
    TitusModule* modules = NULL;

    bool result = SDL_EnumerateDirectory(root, manifest_cb, &modules);
    if(!result) {
        titus_log_error("Failed to enumerate module directory: %s", SDL_GetError());
    }

    return modules;
}

bool titus_load_module_binary(TitusModule* mod) {
    sds bin_path = sdsdup(mod->root_directory);
    bin_path     = sdscatfmt(bin_path, "%S.%s", mod->metadata->name, SHARED_OBJECT_FILE_EXT);

    mod->binary = SDL_LoadObject(bin_path);
    if(NULL == mod->binary) {
        titus_log_error("%s", SDL_GetError());
        return false;
    }

    mod->initialize   = (titus_initialize_proc)SDL_LoadFunction(mod->binary, TITUS_MODULE_INITIALIZE);
    mod->deinitialize = (titus_deinitialize_proc)SDL_LoadFunction(mod->binary, TITUS_MODULE_DEINITIALIZE);

    sdsfree(bin_path);
    return true;
}

bool titus_meta_data_equal(const TitusModuleMetaData* lhs, const TitusModuleMetaData* rhs) {
    if(lhs == NULL && rhs == NULL) {
        return true;
    }

    if(lhs == NULL || rhs == NULL) {
        return false;
    }

    return sdscmp(lhs->namespace, rhs->namespace) == 0 && //
           sdscmp(lhs->name, rhs->name) == 0 &&           //
           titus_version_compare(&lhs->version, &rhs->version) == 0;
}

TitusModule* titus_load_pack_modules(TitusModuleMetaData* required,
                                     size_t required_count,
                                     TitusModule* available,
                                     size_t available_count) {
    TitusModule* modules = NULL;

    for(size_t i = 0; i < required_count; i++) {
        bool found = false;
        for(size_t j = 0; j < available_count; j++) {
            if(titus_meta_data_equal(&required[i], available[j].metadata)) {
                bool result = titus_load_module_binary(&available[j]);
                if(!result) {
                    titus_log_error("Failed to load required module: %s:%s",
                                    available[j].metadata->namespace,
                                    available[j].metadata->name);
                }

                arrpush(modules, available[j]);
                found = true;
                break; // Break out of inner for loop when module is found
            }
        }

        if(!found) {
            titus_log_error("Failed to find required module: %s:%s", required[i].namespace, required[i].name);
        }
    }

    return modules;
}

// Unloads module binary, frees the metadata and pathinfo strings
void titus_free_module(TitusModule* mod) {
    sdsfree(mod->metadata->namespace);
    sdsfree(mod->metadata->name);
    free(mod->metadata);

    SDL_UnloadObject(mod->binary);

    mod->binary       = NULL;
    mod->initialize   = NULL;
    mod->deinitialize = NULL;
}

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
