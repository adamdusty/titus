/* TODO:
    - Module load order
 */

#include "assert/assert.h"
#include "config/config.h"
#include "module/module.h"
#include "titus/ds/stb_ds.h"
#include "titus/sds/sds.h"
#include <SDL3/SDL.h>
#include <flecs.h>
#include <stddef.h>
#include <stdio.h>

typedef int quit_t;

void initialize_logging(const titus_config* config);
void initialize_application_context(titus_application_context* ctx);
void deinitialize_application_context(titus_application_context* ctx);
struct module_kv* load_modules(const titus_config* config, sds executable_direcory);

int main(int, char*[]) {
    SDL_SetAppMetadata("Titus", "0.1.0-alpha", "com.github.titus");
    if(!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
        titus_log_error("Unable to initialize SDL: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    const char* base_path = SDL_GetBasePath();
    if(NULL == base_path) {
        titus_log_error("Failed to find base path of executable");
        exit(EXIT_FAILURE);
    }
    sds executable_directory_path = sdsnew(SDL_GetBasePath());

    titus_config app_config = titus_default_config();
    initialize_logging(&app_config);

    titus_application_context context = {0};
    initialize_application_context(&context);

    /* Set up the explorer */
    ECS_IMPORT(context.ecs, FlecsStats);
    ecs_singleton_set(context.ecs, EcsRest, {0});
    /* ******************* */

    ECS_COMPONENT(context.ecs, quit_t);

    module_kv* modules = load_modules(&app_config, executable_directory_path);
    ecs_entity_t mm    = ecs_lookup(context.ecs, "runtime:module_map");
    ecs_set_id(context.ecs, mm, mm, sizeof(module_kv), &modules);

    for(int i = 0; i < shlen(modules); i++) {
        if(NULL != modules[i].value.initialize)
            modules[i].value.initialize(&context);
    }

    // titus_timer t = {0};
    ecs_singleton_set(context.ecs, quit_t, {1});
    while(*ecs_singleton_get(context.ecs, quit_t) != 0) {
        ecs_progress(context.ecs, 0);
    }

    for(int i = 0; i < shlen(modules); i++) {
        if(NULL != modules[i].value.deinitialize)
            modules[i].value.deinitialize(&context);
    }

    /* Clean up */
    // Deinit modules in reverse order
    for(int i = shlen(modules) - 1; i >= 0; i--) {
        titus_free_module(&modules[i].value);
    }
    shfree(modules);

    sdsfree(executable_directory_path);
    deinitialize_application_context(&context);
    SDL_Quit();
    /* ***** */

    return 0;
}

void initialize_logging(const titus_config* config) {
    sds log_level = sdsnew("app=");
    log_level     = sdscat(log_level, config->log_level);
    titus_log_info("%s", log_level);
#ifdef NDEBUG
    SDL_SetHint(SDL_HINT_LOGGING, log_level);
#else
    SDL_SetHint(SDL_HINT_LOGGING, "app=debug");
#endif
    sdsfree(log_level);
    // SDL_SetLogPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG);
    // SDL_SetLogPriority(SDL_LOG_CATEGORY_VIDEO, SDL_LOG_PRIORITY_TRACE);
    // SDL_SetLogPriorities(SDL_LOG_PRIORITY_TRACE);

    // SDL_IOStream* log = SDL_IOFromFile("log.txt", "a");
    // SDL_SetLogOutputFunction(titus_log_function, log);
}

void initialize_application_context(titus_application_context* ctx) {
    ctx->window = SDL_CreateWindow("Titus", 1280, 720, SDL_WINDOW_RESIZABLE);
    ctx->ecs    = ecs_init();

    TITUS_REGISTER_COMPONENT_NAMED(ctx->ecs, "runtime", "module_map", module_kv*);
    ecs_entity_t window = TITUS_REGISTER_COMPONENT_NAMED(ctx->ecs, "runtime", "window", SDL_Window*);
    ecs_set_id(ctx->ecs, window, window, sizeof(SDL_Window*), &ctx->window);
}
void deinitialize_application_context(titus_application_context* ctx) {
    ecs_fini(ctx->ecs);
    SDL_DestroyWindow(ctx->window);
}

module_kv* load_modules(const titus_config* config, sds executable_directory) {
    TITUS_ASSERT(config != NULL);
    TITUS_ASSERT(config->module_root_directory != NULL);

    sds module_path = sdsempty();
    module_path     = sdscatsds(module_path, executable_directory);
    module_path     = sdscat(module_path, config->module_root_directory);

    titus_module_load_info* module_load_infos = titus_get_module_load_info_from_dir(module_path);
    sdsfree(module_path);

    if(NULL == module_load_infos) {
        titus_log_error("No module loading information found");
        titus_log_error("Runtime requires some core modules to function");
        exit(EXIT_FAILURE);
    }

    // titus_module* modules = NULL;
    module_kv* module_map = NULL;
    for(int i = 0; i < arrlen(module_load_infos); i++) {
        titus_module mod = titus_load_module(&module_load_infos[i]); // module_load_info gets moved into module here
        // arrpush(modules, mod);
        sds key = sdsdup(mod.manifest.namespace);
        key     = sdscat(key, ":");
        key     = sdscatsds(key, mod.manifest.name);
        shput(module_map, key, mod);
    }

    if(NULL == module_map) {
        titus_log_error("No modules loaded");
        titus_log_error("Runtime has no functionality without modules");
        exit(EXIT_FAILURE);
    }

    return module_map;
    // return modules;
}
