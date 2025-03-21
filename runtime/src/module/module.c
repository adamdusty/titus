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

// TODO: Move to serialization
bool titus_parse_manifest(char* data, size_t len, TitusModuleMetaData* out);

/**
 * @brief Callback determines if the current entry is a module.json file. If so, attempts to deserialize it and add it
 * to the module metadata array passed as `user_data`. If the path is a directory, iterate enumerate the subdirectory.
 * Other continue enumerating.
 *
 * @param user_data Pointer to an std dynamic array of TitusModuleMetaData.
 * @param dir Current directory.
 * @param file Current file.
 * @return SDL_EnumerationResult
 */
SDL_EnumerationResult manifest_cb(void* user_data, const char* dir, const char* file) {
    sds path = sdsnew(dir);
    path     = sdscat(path, file);

    SDL_PathInfo info = {0};
    SDL_GetPathInfo(path, &info);

    if(SDL_PATHTYPE_FILE == info.type && SDL_strcmp(MANIFEST_FILE_NAME, file) == 0) {
        TitusModule** modules = user_data;

        TitusModuleMetaData* meta = titus_metadata_new_empty();
        if(!titus_deserialize_manifest(path, meta)) {
            titus_log_error("Failed to deserialize manifest at path: %s", path);

            sdsfree(path);
            return SDL_ENUM_CONTINUE;
        }

        // Unitialized fields are zero initialized here.
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

/**
 * @brief Enumerate through the `root` directory in search of module manifest files. Recursively searches
 * subdirectories.
 *
 * @param root Directory to search for modules.
 * @return TitusModule* An stb dynamic array of TitusModule, with unloaded binary data.
 */
TitusModule* titus_get_available_modules(sds root) {
    TitusModule* modules = NULL;

    SDL_EnumerationResult result = SDL_EnumerateDirectory(root, manifest_cb, &modules);
    if(result != SDL_ENUM_SUCCESS) {
        titus_log_error("Failed to enumerate module directory: %s", SDL_GetError());
    }

    return modules;
}

/**
 * @brief Attempt to load the binary data of a module. Returns false on failure.
 *
 * @param mod Pointer to the module to load.
 * @return true
 * @return false
 */
bool titus_load_module_binary(TitusModule* mod) {
    TITUS_ASSERT(mod->root_directory != NULL); // precondition: The module root directory cannot be null

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

/**
 * @brief Compare to metadata objects. Objects are equal if both are `NULL` or the binary data of the namespaces and
 * names much AND the version numbers match. The version annotation is not considered in the comparison.
 *
 * @param lhs
 * @param rhs
 * @return true
 * @return false
 */
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

/**
 * @brief Searches through available modules for the required modules and loads it. Returns a pointer to a dynamic array
 * of loaded modules.
 *
 * @param required Pointer to an array of metadata for required modules.
 * @param required_count Number of metadata objects in `required`.
 * @param available Pointer to array of available modules.
 * @param available_count Number of module objects in `available`.
 * @return TitusModule*
 */
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

/**
 * @brief Deserialize a module manifest.
 *
 * @param path Path to manifest file.
 * @param out Metadata object to load data into.
 * @return true On success.
 * @return false On failure to load file or failure to parse manifest.
 */
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
        SDL_free(bytes);
        return false;
    }

    SDL_free(bytes);
    return true;
}

/**
 * @brief Deserialize module manifest.
 *
 * @param data Pointer to manifest bytes.
 * @param len Length of manifest bytes.
 * @param out Metadata object to put the manifest data in.
 * @return true On successful deserialization.
 * @return false On unsuccessful deserialization.
 */
bool titus_parse_manifest(char* data, size_t len, TitusModuleMetaData* out) {
    TITUS_ASSERT(data != NULL); // precondition: Attempting to read from NULL
    TITUS_ASSERT(out != NULL);  // precondition: Attempting to write to NULL
    TITUS_ASSERT(len != 0);     // precondition: Zero-length byte array for data

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
            out->namespace = sdsnew(yyjson_get_str(val));
        }

        if(yyjson_equals_str(key, "name")) {
            out->name = sdsnew(yyjson_get_str(val));
        }

        if(yyjson_equals_str(key, "version")) {
            out->version.major      = yyjson_get_uint(yyjson_obj_get(val, "major"));
            out->version.minor      = yyjson_get_uint(yyjson_obj_get(val, "minor"));
            out->version.patch      = yyjson_get_uint(yyjson_obj_get(val, "patch"));
            out->version.annotation = sdsnew(yyjson_get_str(yyjson_obj_get(val, "annotation")));
        }
    }

    yyjson_doc_free(doc);

    return true;
}

/**
 * @brief Allocates a TitusModuleMetaData via `malloc`, and zero-initializes it.
 *
 * @return TitusModuleMetaData*
 */
TitusModuleMetaData* titus_metadata_new_empty() {
    TitusModuleMetaData* meta = malloc(sizeof(TitusModuleMetaData));
    memset(meta, 0, sizeof(TitusModuleMetaData));
    return meta;
}

/**
 * @brief Checks that string fields are not null, and that version field is not all zero with NULL annotation. The
 * version can be 0 with an annotation (e.g. 0.0.0-alpha).
 *
 * @param meta
 * @return sds `NULL` if no missing fields, otherwise the name of the field. The returned pointer should be freed with
 * `sdsfree`.
 */
sds titus_validate_metadata(const TitusModuleMetaData* meta) {
    if(NULL == meta->namespace) {
        return sdsnew("namespace");
    }

    if(NULL == meta->name) {
        return sdsnew("name");
    }

    if(meta->version.major == 0 && meta->version.minor == 0 && meta->version.patch &&
       meta->version.annotation == NULL) {
        return sdsnew("version");
    }

    return NULL;
}
