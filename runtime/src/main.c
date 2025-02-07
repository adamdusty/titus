/*
 */

#include "assert/assert.h"
#include "config/config.h"
#include "ds/stb_ds.h"
#include "log/log.h"
#include "module/module.h"
#include "sds/sds.h"
#include "timer/timer.h"

#include <SDL3/SDL.h>
#include <flecs.h>
#include <stddef.h>
#include <stdio.h>

void initialize_logging(const titus_config* config);
void initialize_application_context(titus_application_context* ctx);
void deinitialize_application_context(titus_application_context* ctx);
titus_module* load_modules(const titus_config* config, sds executable_direcory);

typedef struct quit {
    bool should_quit;
} quit;
ECS_COMPONENT_DECLARE(quit);

typedef struct core_frame_input {
    size_t count;
    SDL_Event events[255];
} core_frame_input;
void process_input(ecs_iter_t* it) {
    core_frame_input* fi = ecs_field(it, core_frame_input, 0);

    for(int i = 0; i < it->count; i++) {
        for(size_t j = 0; j < fi->count; j++) {

            switch(fi->events[j].type) {
            case SDL_EVENT_KEY_DOWN: {
                printf("Key pressed!: %d", fi->events[j].key.scancode);
                if(fi->events[j].key.scancode == SDL_SCANCODE_ESCAPE) {
                    printf("quit\n");
                    ecs_singleton_set(it->world, quit, {.should_quit = true});
                }
                break;
            }
            }
        }
    }
}

int main(int, char*[]) {
    SDL_SetAppMetadata("Titus", "0.1.0-alpha", "com.github.titus");
    if(!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
        log_error("Unable to initialize SDL: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    const char* base_path = SDL_GetBasePath();
    if(NULL == base_path) {
        log_error("Failed to find base path of executable");
        exit(EXIT_FAILURE);
    }
    sds executable_directory_path = sdsnew(SDL_GetBasePath());

    titus_config app_config = titus_default_config();
    initialize_logging(&app_config);

    titus_application_context context = {0};
    initialize_application_context(&context);

    titus_module* modules = load_modules(&app_config, executable_directory_path);
    for(int i = 0; i < arrlen(modules); i++) {
        modules[i].initialize(&context);
    }

    // ECS_SYSTEM(context.ecs, process_input, EcsPreUpdate, quit);

    // titus_timer t = {0};
    ECS_COMPONENT_DEFINE(context.ecs, quit);
    ecs_system(context.ecs,
               {
                   .callback = process_input,
                   .entity   = ecs_entity(context.ecs,
                                          {
                                              .name = "process_input",
                                              .add  = ecs_ids(ecs_dependson(EcsOnUpdate)),
                                        }),
                   .query.terms =
                       {
                           {.first.name = "core:input"},
                       },
               });
    ecs_singleton_set(context.ecs, quit, {false});
    while(!ecs_singleton_get(context.ecs, quit)->should_quit) {
        ecs_progress(context.ecs, 0);
    }

    for(int i = 0; i < arrlen(modules); i++) {
        modules[i].deinitialize(&context);
    }

    /* Clean up */
    for(int i = 0; i < arrlen(modules); i++) {
        titus_free_module(&modules[i]);
    }
    arrfree(modules);

    sdsfree(executable_directory_path);
    deinitialize_application_context(&context);
    SDL_Quit();
    /* ***** */

    return 0;
}

void initialize_logging(const titus_config* config) {
    sds log_level = sdsnew("app=");
    log_level     = sdscat(log_level, config->log_level);
    log_info("%s", log_level);
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

typedef struct x {
    int y;
} x;

typedef x* x_p;

void initialize_application_context(titus_application_context* ctx) {
    ctx->window = SDL_CreateWindow("Titus", 640, 480, 0);
    ctx->ecs    = ecs_init();

    ecs_component(ctx->ecs,
                  {
                      .entity = ecs_entity(ctx->ecs, {.name = "SDL_Window"}),
                      .type   = {.size = sizeof(SDL_Window*), .alignment = alignof(SDL_Window*)},
                  });
}
void deinitialize_application_context(titus_application_context* ctx) {
    ecs_fini(ctx->ecs);
    SDL_DestroyWindow(ctx->window);
}

titus_module* load_modules(const titus_config* config, sds executable_directory) {
    TITUS_ASSERT(config != NULL);
    TITUS_ASSERT(config->module_root_directory != NULL);

    sds module_path = sdsempty();
    module_path     = sdscatsds(module_path, executable_directory);
    module_path     = sdscat(module_path, config->module_root_directory);

    titus_module_load_info* module_load_infos = titus_get_module_load_info_from_dir(module_path);
    sdsfree(module_path);

    if(NULL == module_load_infos) {
        log_error("No module loading information found");
        log_error("Runtime requires some core modules to function");
        exit(EXIT_FAILURE);
    }

    titus_module* modules = NULL;
    for(int i = 0; i < arrlen(module_load_infos); i++) {
        titus_module mod = titus_load_module(&module_load_infos[i]); // module_load_info gets moved into module here
        arrpush(modules, mod);
    }

    if(NULL == modules) {
        log_error("No modules loaded");
        log_error("Runtime has no functionality without modules");
        exit(EXIT_FAILURE);
    }

    return modules;
}
