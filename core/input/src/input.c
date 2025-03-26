#include "input/export.h"
#include <SDL3/SDL.h>
#include <core/core.h>
#include <flecs.h>
#include <titus/sdk.h>

void core_input_poll_system(ecs_iter_t* it);
void process_input(ecs_iter_t* it);

ECS_SYSTEM_DECLARE(core_input_poll_system);
ECS_SYSTEM_DECLARE(process_input);

CORE_INPUT_EXPORT void inputImport(ecs_world_t* ecs) {
    ECS_MODULE(ecs, CoreInput);

    ECS_SYSTEM_DEFINE(ecs, core_input_poll_system, EcsPreUpdate, core.CoreFrameInput);
    ECS_SYSTEM_DEFINE(ecs, process_input, EcsOnUpdate, core.CoreFrameInput);
}

CORE_INPUT_EXPORT void titus_initialize(TitusApplicationContext* ctx, sds /* root */) {
    titus_log_info("Initializing core:input module");

    ecs_entity_t mod = ecs_import(ctx->ecs, inputImport, "core.input");
    if(mod == 0) {
        titus_log_error("Failed to import core.input module");
        return;
    }

    titus_log_info("core.input module imported");

    // Attach input component to world as singleton
    ecs_singleton_set(ctx->ecs, CoreFrameInput, {.count = 0, .events = {0}});
}

CORE_INPUT_EXPORT void titus_deinitialize(TitusApplicationContext* ctx) {}

void core_input_poll_system(ecs_iter_t* it) {
    CoreFrameInput* inp = ecs_field(it, CoreFrameInput, 0);

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
    CoreFrameInput* fi = ecs_field(it, CoreFrameInput, 0);

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
