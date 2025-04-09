/* TODO:
    - Module load order
 */

#include "config/config.h"
#include "module/module.h"
#include "module/pack.h"
#include <SDL3/SDL.h>
#include <flecs.h>
#include <stddef.h>
#include <stdio.h>
#include <titus/sdk.h>

typedef int quit_t;

void initialize_logging(const TitusConfig* config);
void initialize_application_context(TitusApplicationContext* ctx);
void deinitialize_application_context(TitusApplicationContext* ctx);

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
    sds module_directory          = sdsdup(executable_directory_path);
    sds pack_directory            = sdsdup(executable_directory_path);

    TitusConfig app_config = titus_default_config();
    initialize_logging(&app_config);

    TitusApplicationContext context = {0};
    initialize_application_context(&context);

    /* Set up the explorer */
    ECS_IMPORT(context.ecs, FlecsStats);
    ecs_singleton_set(context.ecs, EcsRest, {0});
    /* ******************* */

    ECS_COMPONENT(context.ecs, quit_t);

    // Find all available modules
    module_directory               = sdscatfmt(module_directory, "/%s", app_config.module_directory);
    TitusModule* available_modules = titus_get_available_modules(module_directory);
    if(NULL == available_modules) {
        titus_log_error("No modules found");
        exit(EXIT_FAILURE);
    }

    for(int i = 0; i < arrlen(available_modules); i++) {
        titus_log_debug("%s:%s", available_modules[i].metadata->namespace, available_modules[i].metadata->name);
    }

    // Find all pack files
    pack_directory         = sdscatfmt(pack_directory, "/%s", app_config.pack_directory);
    TitusModulePack* packs = titus_get_module_packs(pack_directory);
    if(packs == NULL) {
        titus_log_error("No valid packs found.");
        exit(EXIT_FAILURE);
    }

    // TODO: Allow user to choose pack or set default pack(?)
    // TODO: Verify that all modules in pack are available
    TitusModulePack* pack = &packs[0];

    // Load modules for pack
    TitusModule* modules =
        titus_load_pack_modules(pack->modules, pack->module_count, available_modules, arrlenu(available_modules));
    if(NULL == modules) {
        titus_log_error("Failed to load modules for pack");
        exit(EXIT_FAILURE);
    }

    // Initialize modules and start applciation
    for(int i = 0; i < arrlen(modules); i++) {
        if(NULL != modules[i].initialize)
            modules[i].initialize(&context, modules[i].root_directory);
    }

    // titus_timer t = {0};
    ecs_singleton_set(context.ecs, quit_t, {1});
    while(*ecs_singleton_get(context.ecs, quit_t) != 0) {
        ecs_progress(context.ecs, 0);
    }

    for(ptrdiff_t i = arrlen(modules) - 1; i >= 0; i--) {
        if(NULL != modules[i].deinitialize)
            modules[i].deinitialize(&context);
    }

    /* Clean up */
    // Deinit modules in reverse order
    for(ptrdiff_t i = arrlen(modules) - 1; i >= 0; i--) {
        titus_log_info("Freeing module: %s:%s", modules[i].metadata->namespace, modules[i].metadata->name);
        titus_free_module(&modules[i]);
    }
    arrfree(modules);

    sdsfree(executable_directory_path);
    deinitialize_application_context(&context);
    SDL_Quit();
    /* ***** */

    return 0;
}

void initialize_logging(const TitusConfig* config) {
    sds log_level = sdsnew("app=");
    log_level     = sdscat(log_level, config->log_level);
    titus_log_info("%s", log_level);
#ifdef NDEBUG
    SDL_SetHint(SDL_HINT_LOGGING, log_level);
#else
    SDL_SetHint(SDL_HINT_LOGGING, "app=debug");
#endif
    // clang-format off
    SDL_SetLogPriorityPrefix(SDL_LOG_PRIORITY_VERBOSE, "[VERBOSE] ");
    SDL_SetLogPriorityPrefix(SDL_LOG_PRIORITY_TRACE,   "[TRACE]   ");
    SDL_SetLogPriorityPrefix(SDL_LOG_PRIORITY_DEBUG,   "[DEBUG]   ");
    SDL_SetLogPriorityPrefix(SDL_LOG_PRIORITY_INFO,    "[INFO]    ");
    SDL_SetLogPriorityPrefix(SDL_LOG_PRIORITY_WARN,    "[WARN]    ");
    SDL_SetLogPriorityPrefix(SDL_LOG_PRIORITY_ERROR,   "[ERROR]   ");
    // clang-format on
    sdsfree(log_level);
    // SDL_SetLogPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG);
    // SDL_SetLogPriority(SDL_LOG_CATEGORY_VIDEO, SDL_LOG_PRIORITY_TRACE);
    // SDL_SetLogPriorities(SDL_LOG_PRIORITY_TRACE);

    // SDL_IOStream* log = SDL_IOFromFile("log.txt", "a");
    // SDL_SetLogOutputFunction(titus_log_function, log);
}

void initialize_application_context(TitusApplicationContext* ctx) {
    ctx->window = SDL_CreateWindow("Titus", 1280, 720, SDL_WINDOW_RESIZABLE);
    ctx->ecs    = ecs_init();

    ecs_entity_t window = ecs_component(ctx->ecs,
                                        {
                                            .type.alignment = alignof(SDL_Window*),
                                            .type.size      = sizeof(SDL_Window*),
                                            .entity         = ecs_entity(ctx->ecs, {.name = "runtime:window"}),
                                        });

    ecs_set_id(ctx->ecs, window, window, sizeof(SDL_Window*), &ctx->window);
}

void deinitialize_application_context(TitusApplicationContext* ctx) {
    ecs_fini(ctx->ecs);
    SDL_DestroyWindow(ctx->window);
}
