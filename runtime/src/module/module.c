#include "module/module.h"

#include "assert/assert.h"
#include "ds/stb_ds.h"
#include "log/log.h"
#include "sds/sds.h"
#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <yyjson.h>

// #ifdef __linux__
// #define SHARED_OBJECT_FILE_EXT "so"
// #elifdef _WIN32
// #define SHARED_OBJECT_FILE_EXT "dll"
// #endif

static const char* MANIFEST_FILE_NAME = "module.json";

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

    if(SDL_strcmp(f, MANIFEST_FILE_NAME) != 0) {
        return SDL_ENUM_CONTINUE;
    }

    sds path = sdsnew(d);
    path     = sdscat(path, f);

    sds** paths = ud;
    arrpush(*paths, path);

    return SDL_ENUM_CONTINUE;
}

SDL_EnumerationResult version_callback(void* ud, const char* d, const char* f) {
    // Enumerate over directories in the version level of the folder
    // Callback for each manifest file in this level
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

SDL_EnumerationResult name_callback(void* ud, const char* d, const char* f) {
    // Enumerate over directories in the name level of the folder
    // Callback for each module version in this level
    sds path = sdsnew(d);
    path     = sdscat(path, f);

    if(NULL == path) {
        return SDL_ENUM_FAILURE;
    }
    SDL_PathInfo info = {0};
    SDL_GetPathInfo(path, &info);

    if(SDL_PATHTYPE_DIRECTORY == info.type) {
        SDL_EnumerateDirectory(path, version_callback, ud);
    }

    sdsfree(path);
    return SDL_ENUM_CONTINUE;
}

SDL_EnumerationResult namespace_callback(void* ud, const char* d, const char* f) {
    // Enumerate over directories in the namespace level of the folder
    // Callback for each module name in this level
    sds path = sdsnew(d);
    path     = sdscat(path, f);

    if(NULL == path) {
        return SDL_ENUM_FAILURE;
    }
    SDL_PathInfo info = {0};
    SDL_GetPathInfo(path, &info);

    if(SDL_PATHTYPE_DIRECTORY == info.type) {
        SDL_EnumerateDirectory(path, name_callback, ud);
    }

    sdsfree(path);
    return SDL_ENUM_CONTINUE;
}

sds* titus_get_manifest_paths_from_root(const char* root) {
    TITUS_ASSERT(NULL != root); // precondition: Attempting to read from NULL

    sds* paths  = NULL;
    bool result = SDL_EnumerateDirectory(root, namespace_callback, &paths);
    if(!result) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to enumerate directory: %s", SDL_GetError());
    }

    return paths;
}

bool titus_load_manifest_from_path(const char* path, titus_module_manifest* out) {
    TITUS_ASSERT(path != NULL); // precondition: Attempting to load from empty path
    TITUS_ASSERT(out != NULL);  // precondition: Attempting to write to NULL

    // Check if path exists
    if(!SDL_GetPathInfo(path, NULL)) {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Manifest file does not exists at path: %s", path);
        return false;
    }

    size_t data_size = 0;
    char* data       = SDL_LoadFile(path, &data_size);

    return titus_parse_manifest(data, data_size, out);
}

bool titus_parse_manifest(char* data, size_t len, titus_module_manifest* out) {
    TITUS_ASSERT(out != NULL); // precondition: Attempting to write to NULL

    if(NULL == data || len == 0) {
        log_error("Empty manifest");
        return false;
    }

    yyjson_read_err err;
    yyjson_read_flag flags = YYJSON_READ_ALLOW_COMMENTS | YYJSON_READ_ALLOW_TRAILING_COMMAS;
    yyjson_doc* doc        = yyjson_read_opts(data, len, flags, NULL, &err);

    if(NULL == doc) {
        log_error("Failed to read json document");

        size_t line = 0;
        size_t col  = 0;
        size_t ch   = 0;
        if(yyjson_locate_pos(data, len, err.pos, &line, &col, &ch)) {
            log_error("%s: [%llu: %llu]", err.msg, line, col);
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
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to parse valid manifest: %s", msg);
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
