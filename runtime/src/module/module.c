// TODO: Add directory path to module on load

#include "module/module.h"

#include "assert/assert.h"
#include "sds/sds.h"
#include "titus/ds/stb_ds.h"
#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <yyjson.h>

#ifdef __linux__
#define SHARED_OBJECT_FILE_EXT "so"
#elifdef _WIN32
#define SHARED_OBJECT_FILE_EXT "dll"
#endif

static const char* MANIFEST_FILE_NAME = "module.json";

void titus_free_module(titus_module* mod) {
    sdsfree(mod->directory_path);
    sdsfree(mod->binary_path);
    sdsfree(mod->resource_path);
    SDL_UnloadObject(mod->handle);

    mod->handle         = NULL;
    mod->initialize     = NULL;
    mod->deinitialize   = NULL;
    mod->directory_path = NULL;
    mod->binary_path    = NULL;
    mod->resource_path  = NULL;
}

sds titus_version_to_string(const titus_module_version* ver) {
    TITUS_ASSERT(ver != NULL); // precondition: Attempting to read from NULL

    sds str = sdsempty();
    if(ver->annotation != NULL && sdslen(ver->annotation) > 0) {
        str = sdscatfmt(str, "%U.%U.%U-%s", ver->major, ver->minor, ver->patch, ver->annotation);
    } else {
        str = sdscatfmt(str, "%U.%U.%U", ver->major, ver->minor, ver->patch);
    }

    return str;
}

sds titus_module_dir_from_manifest(const titus_module_manifest* man) {
    TITUS_ASSERT(NULL != man);            // precondition: Attempting to read from NULL
    TITUS_ASSERT(NULL != man->namespace); // precondition: Attempting to read from NULL
    TITUS_ASSERT(NULL != man->name);      // precondition: Attempting to read from NULL

    sds path = sdsempty();
    path     = sdscatfmt(path, "%s/%s/", man->namespace, man->name);
    sds ver  = titus_version_to_string(&man->version);
    path     = sdscatsds(path, ver);
    sdsfree(ver);

    return path;
}

SDL_EnumerationResult manifest_callback(void* ud, const char* d, const char* f) {
    TITUS_ASSERT(NULL != ud); // precondition: Attempting to write to NULL
    titus_log_verbose("Enumerating directory %s: %s", d, f);

    if(SDL_strcmp(f, MANIFEST_FILE_NAME) != 0) {
        return SDL_ENUM_CONTINUE;
    }

    sds path = sdsnew(d);
    path     = sdscat(path, f);

    titus_module_manifest man = {0};
    if(!titus_load_manifest_from_path(path, &man)) {
        titus_log_error("Found module.json (%s), but failed to load", path);
        return SDL_ENUM_CONTINUE;
    }
    titus_module_load_info li = {.manifest = man, .binary = NULL, .resources = NULL};

    if(li.manifest.binary) {
        li.binary = sdsnew(d);
        li.binary = sdscatfmt(li.binary, "%S.%s", man.binary, SHARED_OBJECT_FILE_EXT);
    }

    sds res = sdsnew(d);
    res     = sdscat(res, "resources");
    if(SDL_GetPathInfo(res, NULL)) {
        titus_log_debug("Resource folder found for %s:%s", li.manifest.namespace, li.manifest.name);
        li.resources = res;
    } else {
        titus_log_debug("No resource folder found for module: %s:%s", li.manifest.namespace, li.manifest.name);
        titus_log_debug("Searched for resource folder at: %s", res);
        sdsfree(res);
    }

    titus_module_load_info** li_arr = ud;
    arrpush(*li_arr, li);

    titus_log_debug("Successfully gathered load info from %s", d);

    return SDL_ENUM_CONTINUE;
}

SDL_EnumerationResult modules_callback(void* ud, const char* d, const char* f) {
    // Enumerate over directories in the namespace level of the folder
    // Callback for each module name in this level
    titus_log_verbose("Enumerating directory %s: %s", d, f);

    sds path = sdsnew(d);
    path     = sdscat(path, f);

    if(NULL == path) {
        return SDL_ENUM_FAILURE;
    }
    SDL_PathInfo info = {0};
    SDL_GetPathInfo(path, &info);

    if(SDL_PATHTYPE_DIRECTORY == info.type) {
        SDL_EnumerateDirectory(path, manifest_callback, ud);
    }

    sdsfree(path);
    return SDL_ENUM_CONTINUE;
}

sds* titus_get_manifest_paths_from_root(const char* root) {
    TITUS_ASSERT(NULL != root); // precondition: Attempting to read from NULL

    sds* paths  = NULL;
    bool result = SDL_EnumerateDirectory(root, modules_callback, &paths);
    if(!result) {
        titus_log_error("Error while enumerating directory: %s", SDL_GetError());
    }

    return paths;
}

bool titus_load_manifest_from_path(const char* path, titus_module_manifest* out) {
    TITUS_ASSERT(path != NULL); // precondition: Attempting to load from empty path
    TITUS_ASSERT(out != NULL);  // precondition: Attempting to write to NULL

    // Check if path exists
    if(!SDL_GetPathInfo(path, NULL)) {
        titus_log_warn("Manifest file does not exists at path: %s", path);
        return false;
    }

    size_t data_size = 0;
    char* data       = SDL_LoadFile(path, &data_size);

    return titus_parse_manifest(data, data_size, out);
}

bool titus_parse_manifest(char* data, size_t len, titus_module_manifest* out) {
    TITUS_ASSERT(out != NULL); // precondition: Attempting to write to NULL

    if(NULL == data || len == 0) {
        titus_log_error("Empty manifest");
        return false;
    }

    yyjson_read_err err;
    yyjson_read_flag flags = YYJSON_READ_ALLOW_COMMENTS | YYJSON_READ_ALLOW_TRAILING_COMMAS;
    yyjson_doc* doc        = yyjson_read_opts(data, len, flags, NULL, &err);

    if(NULL == doc) {
        titus_log_error("Failed to read json document");

        size_t line = 0;
        size_t col  = 0;
        size_t ch   = 0;
        if(yyjson_locate_pos(data, len, err.pos, &line, &col, &ch)) {
            titus_log_error("%s: [%llu: %llu]", err.msg, line, col);
        }
        return false;
    }

    yyjson_val* root = yyjson_doc_get_root(doc);

    // manifest root should be an object
    if(yyjson_get_type(root) != YYJSON_TYPE_OBJ) {
        yyjson_doc_free(doc);
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

        if(yyjson_equals_str(key, "binary")) {
            out->binary = sdsnew(yyjson_get_str(val));
        }

        if(yyjson_equals_str(key, "version")) {
            out->version.major      = yyjson_get_int(yyjson_obj_get(val, "major"));
            out->version.minor      = yyjson_get_int(yyjson_obj_get(val, "minor"));
            out->version.patch      = yyjson_get_int(yyjson_obj_get(val, "patch"));
            out->version.annotation = sdsnew(yyjson_get_str(yyjson_obj_get(val, "annotation")));
        }
    }

    char* msg = NULL;
    if(!titus_validate_manifest(out, &msg)) {
        titus_log_error("Failed to parse valid manifest: %s", msg);
        yyjson_doc_free(doc);
        return false;
    }

    yyjson_doc_free(doc);
    return true;
}

bool titus_validate_manifest(titus_module_manifest* man, char** msg) {
    TITUS_ASSERT(man != NULL); // precondition: Attempting to read from NULL

    if(man->namespace == NULL || sdslen(man->namespace) == 0) {
        *msg = sdsnew("Missing required field: namespace");
        return false;
    }

    if(man->name == NULL || sdslen(man->name) == 0) {
        *msg = sdsnew("Missing required field: name");
        return false;
    }

    return true;
}

titus_module_load_info* titus_get_module_load_info_from_dir(const char* root) {
    TITUS_ASSERT(NULL != root); // precondition: Attempting to read from NULL

    titus_module_load_info* loads = NULL;

    bool result = SDL_EnumerateDirectory(root, modules_callback, &loads);
    if(!result) {
        titus_log_error("Error while enumerating directory: %s", SDL_GetError());
    }

    return loads;
}

titus_module titus_load_module(titus_module_load_info* load_info) {
    TITUS_ASSERT(load_info != NULL); // precondition: NULL load_info

    titus_module module = {0};
    module.manifest     = load_info->manifest;

    if(SDL_GetPathInfo(load_info->binary, NULL)) {
        module.binary_path = load_info->binary;
        module.handle      = SDL_LoadObject(load_info->binary);

        if(NULL == module.handle) {
            titus_log_error("Failed to load shared object at %s", load_info->binary);
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
