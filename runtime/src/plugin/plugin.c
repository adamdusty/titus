#include "plugin/plugin.h"

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

SDL_EnumerationResult manifest_callback(void* ud, const char* d, const char* f) {
    // Enumerate over directories in the version level of the folder
    // Callback for each manifest file in this level
    if(SDL_strcmp(f, MANIFEST_FILE_NAME) != 0) {
        return SDL_ENUM_CONTINUE;
    }

    size_t path_size = SDL_strlen(d) + SDL_strlen(f) + 1;
    char path[path_size];
    if(SDL_snprintf(path, path_size, "%s%s", d, f) > path_size) {
        SDL_snprintf(ud, ERROR_MSG_SIZE, "Path size was truncated during enumeration: %s", path);
        return SDL_ENUM_FAILURE;
    }
    SDL_PathInfo info = {0};
    SDL_GetPathInfo(path, &info);
    SDL_Log("%s%s", d, f);
    return SDL_ENUM_CONTINUE;
}

SDL_EnumerationResult name_callback(void* ud, const char* d, const char* f) {
    // Enumerate over directories in the name level of the folder
    // Callback for each plugin version in this level
    size_t path_size = SDL_strlen(d) + SDL_strlen(f) + 1;
    char path[path_size];
    if(SDL_snprintf(path, path_size, "%s%s", d, f) > path_size) {
        SDL_snprintf(ud, ERROR_MSG_SIZE, "Path size was truncated during enumeration: %s", path);
        return SDL_ENUM_FAILURE;
    }
    SDL_PathInfo info = {0};
    SDL_GetPathInfo(path, &info);

    if(SDL_PATHTYPE_DIRECTORY == info.type) {
        SDL_EnumerateDirectory(path, manifest_callback, ud);
    }

    return SDL_ENUM_CONTINUE;
}

SDL_EnumerationResult namespace_callback(void* ud, const char* d, const char* f) {
    // Enumerate over directories in the namespace level of the folder
    // Callback for each plugin name in this level
    size_t path_size = SDL_strlen(d) + SDL_strlen(f) + 1;
    char path[path_size];
    if(SDL_snprintf(path, path_size, "%s%s", d, f) > path_size) {
        SDL_snprintf(ud, ERROR_MSG_SIZE, "Path size was truncated during enumeration: %s", path);
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
    char msg[ERROR_MSG_SIZE] = {0};
    SDL_EnumerateDirectory(root, namespace_callback, msg);
    return titus_result_ok();
}
