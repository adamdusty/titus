#include "input/export.h"
#include <SDL3/SDL.h>
#include <core/core.h>
#include <flecs.h>
#include <titus/sdk.h>

void core_input_poll_system(ecs_iter_t* it);

ECS_SYSTEM_DECLARE(core_input_poll_system);
ECS_SYSTEM_DECLARE(process_input);

CORE_INPUT_EXPORT void inputImport(ecs_world_t* ecs) {
    ECS_MODULE(ecs, CoreInput);

    ECS_SYSTEM_DEFINE(ecs, core_input_poll_system, EcsPreUpdate, core.CoreFrameInput);
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
    ecs_singleton_set(ctx->ecs, CoreFrameInput, {0});
}

CORE_INPUT_EXPORT void titus_deinitialize(TitusApplicationContext* ctx) {}

void core_input_poll_system(ecs_iter_t* it) {
    CoreFrameInput* inp = ecs_field(it, CoreFrameInput, 0);

    inp->state[1] = inp->state[0];
    SDL_memset(&inp->state[0], 0, sizeof(CoreFrameInputState));

    for(int i = 0; i < it->count; i++) {
        SDL_Event e;
        while(SDL_PollEvent(&e)) {
            switch(e.type) {
            case SDL_EVENT_KEY_DOWN: {
                inp->state[0].scancodes[e.key.scancode] = CORE_KEY_STATE_DOWN;
                break;
            }
            case SDL_EVENT_KEY_UP: {
                inp->state[0].scancodes[e.key.scancode] = CORE_KEY_STATE_UP;
                break;
            }
            case SDL_EVENT_MOUSE_BUTTON_DOWN: {
                inp->state[0].mouse_buttons[e.button.button] = CORE_BUTTON_STATE_DOWN;
                break;
            }
            case SDL_EVENT_MOUSE_BUTTON_UP: {
                inp->state[0].mouse_buttons[e.button.button] = CORE_BUTTON_STATE_UP;
                break;
            }
            default: {
                break;
            }
            }
        }
    }
}
