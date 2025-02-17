#include "core/export.h"
#include <SDL3/SDL.h>
#include <core/components.h>
#include <flecs.h>
#include <titus/sdk.h>

void core_input_poll_system(ecs_iter_t* it);
void process_input(ecs_iter_t* it);

ECS_SYSTEM_DECLARE(core_input_poll_system);
ECS_SYSTEM_DECLARE(process_input);

extern void coreImport(ecs_world_t* world);

CORE_EXPORT void inputImport(ecs_world_t* ecs) {
    ecs_entity_t input_module = 0;
    ecs_component_desc_t desc = {0};
    desc.entity               = input_module;
    ecs_module_init(ecs, "core.input", &desc);
    ecs_set_scope(ecs, input_module);

    ECS_SYSTEM_DEFINE(ecs, core_input_poll_system, EcsPreUpdate, core.core_frame_input);
    ECS_SYSTEM_DEFINE(ecs, process_input, EcsOnUpdate, core.core_frame_input);
}

CORE_EXPORT void titus_initialize(titus_application_context* ctx) {
    titus_log_info("Initializing core:input module");

    ecs_entity_t comp = ecs_lookup(ctx->ecs, "core.core_frame_input");

    // Attach input component to world as singleton
    ecs_set_id(ctx->ecs, comp, comp, sizeof(core_frame_input), &(core_frame_input){.count = 0, .events = {0}});

    ecs_import(ctx->ecs, inputImport, "core.input");
}

CORE_EXPORT void titus_deinitialize(titus_application_context* ctx) {}

void core_input_poll_system(ecs_iter_t* it) {
    core_frame_input* inp = ecs_field(it, core_frame_input, 0);

    memset(inp->events, 0, 255 * sizeof(SDL_Event));

    for(int i = 0; i < it->count; i++) {
        SDL_Event e;
        size_t index = 0;
        while(SDL_PollEvent(&e) && index < 255) {
            inp[i].events[index++] = e;
        }
        inp->count = index;
    }
}

void process_input(ecs_iter_t* it) {
    core_frame_input* fi = ecs_field(it, core_frame_input, 0);

    ecs_entity_t q         = ecs_lookup(it->world, "quit_t");
    const EcsComponent* qc = ecs_get_id(it->world, q, ecs_id(EcsComponent));

    for(int i = 0; i < it->count; i++) {
        for(size_t j = 0; j < fi->count; j++) {
            switch(fi->events[j].type) {
            case SDL_EVENT_QUIT: {
                ecs_set_id(it->world, q, q, qc->size, &(int){0});
                break;
            }
            case SDL_EVENT_KEY_DOWN: {
                if(fi->events[j].key.scancode == SDL_SCANCODE_ESCAPE) {
                    ecs_set_id(it->world, q, q, qc->size, &(int){0});
                }
                break;
            }
            }
        }
    }
}
