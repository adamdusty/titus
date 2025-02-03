#include "plugin/plugin.h"

#include "sds/sds.h"
#include <SDL3/SDL.h>
#include <stdlib.h>
#include <string.h>

#define ERROR_MSG_SIZE 1024

static const size_t GROWTH            = 2;
static const char* MANIFEST_FILE_NAME = "plugin.json";

titus_plugin_array titus_plugin_array_new() {
    titus_plugin_array arr = {.plugins = NULL, .count = 0, .capacity = 8};
    arr.plugins            = malloc(sizeof(titus_plugin) * 8);
    return arr;
}

void titus_plugin_array_free(titus_plugin_array* arr) {
    free(arr->plugins);
    arr->plugins  = NULL;
    arr->count    = 0;
    arr->capacity = 0;
}

titus_result titus_plugin_array_append(titus_plugin_array* arr, titus_plugin plugin) {
    if(arr->count == arr->capacity) {
        titus_plugin* old = arr->plugins;
        size_t new_cap    = arr->capacity * GROWTH;
        arr->plugins      = malloc(new_cap);

        if(arr->plugins == NULL) {
            arr->plugins = old;
            return titus_result_err("Unable to allocate more memory for underlying array");
        }

        if(arr->plugins != NULL) {
            memcpy(arr->plugins, old, sizeof(titus_plugin) * arr->count);
            free(old);
        }
    }

    // Copy plugin into heap memory of array
    arr->plugins[arr->count++] = plugin;
    return titus_result_ok();
}

struct plugin_paths {
    size_t count;
    sds* paths;
};

SDL_EnumerationResult manifest_callback(void* ud, const char* d, const char* f) {
    SDL_Log("Enumerating manifests");
    if(SDL_strcmp(f, MANIFEST_FILE_NAME) != 0) {
        // SDL_Log("%s%s", d, f);
        return SDL_ENUM_CONTINUE;
    }

    sds path = sdsnew(d);
    path     = sdscat(path, f);

    SDL_PathInfo info = {0};
    SDL_GetPathInfo(path, &info);
    struct plugin_paths* p = ud;
    p->paths[p->count++]   = path;

    return SDL_ENUM_CONTINUE;
}

SDL_EnumerationResult version_callback(void* ud, const char* d, const char* f) {
    // Enumerate over directories in the version level of the folder
    // Callback for each manifest file in this level
    SDL_Log("Enumerating versions");
    size_t path_size = SDL_strlen(d) + SDL_strlen(f) + 1;
    char path[path_size];
    if(SDL_snprintf(path, path_size, "%s%s", d, f) > path_size) {
        char msg[1024];
        SDL_snprintf(msg, ERROR_MSG_SIZE, "Path size was truncated during enumeration: %s", path);
        SDL_Log("%s", msg);
        return SDL_ENUM_FAILURE;
    }
    SDL_PathInfo info = {0};
    SDL_GetPathInfo(path, &info);

    if(SDL_PATHTYPE_DIRECTORY == info.type) {
        SDL_EnumerateDirectory(path, manifest_callback, ud);
    }

    return SDL_ENUM_CONTINUE;
}

SDL_EnumerationResult name_callback(void* ud, const char* d, const char* f) {
    // Enumerate over directories in the name level of the folder
    // Callback for each plugin version in this level
    SDL_Log("Enumerating names");
    size_t path_size = SDL_strlen(d) + SDL_strlen(f) + 1;
    char path[path_size];
    if(SDL_snprintf(path, path_size, "%s%s", d, f) > path_size) {
        char msg[1024];
        SDL_snprintf(msg, ERROR_MSG_SIZE, "Path size was truncated during enumeration: %s", path);
        SDL_Log("%s", msg);
        return SDL_ENUM_FAILURE;
    }
    SDL_PathInfo info = {0};
    SDL_GetPathInfo(path, &info);

    if(SDL_PATHTYPE_DIRECTORY == info.type) {
        SDL_EnumerateDirectory(path, version_callback, ud);
    }

    return SDL_ENUM_CONTINUE;
}

SDL_EnumerationResult namespace_callback(void* ud, const char* d, const char* f) {
    // Enumerate over directories in the namespace level of the folder
    // Callback for each plugin name in this level
    SDL_Log("Enumerating namespaces");
    size_t path_size = SDL_strlen(d) + SDL_strlen(f) + 1;
    char path[path_size];
    if(SDL_snprintf(path, path_size, "%s%s", d, f) > path_size) {
        char msg[1024];
        SDL_snprintf(msg, ERROR_MSG_SIZE, "Path size was truncated during enumeration: %s", path);
        SDL_Log("%s", msg);
        return SDL_ENUM_FAILURE;
    }
    SDL_PathInfo info = {0};
    SDL_GetPathInfo(path, &info);

    if(SDL_PATHTYPE_DIRECTORY == info.type) {
        SDL_EnumerateDirectory(path, name_callback, ud);
    }

    return SDL_ENUM_CONTINUE;
}

titus_result load_plugins(titus_plugin_array* arr, const char* root) {
    struct plugin_paths paths = {.count = 0, .paths = NULL};
    paths.paths               = malloc(sizeof(sds) * 65536);
    if(NULL == paths.paths) {
        return titus_result_err("Failed to allocate memory for path buffer");
    }

    SDL_EnumerateDirectory(root, namespace_callback, &paths);
    SDL_Log("plugin count: %llu", paths.count);
    for(int i = 0; i < paths.count; i++) {
        SDL_Log("path: %s", paths.paths[i]);
    }
    return titus_result_ok();
}
