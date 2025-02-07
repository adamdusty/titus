#include "input/export.h"
#include <SDL3/SDL.h>
#include <flecs.h>
#include <titus/sdk.h>

typedef struct core_frame_input {
    size_t count;
    SDL_Event events[255];
} core_frame_input;

void core_input_poll_system(ecs_iter_t* it) {
    core_frame_input* inp = ecs_field(it, core_frame_input, 0);

    memset(inp->events, 0, 255 * sizeof(SDL_Event));

    for(int i = 0; i < it->count; i++) {
        SDL_Event e;
        size_t index = 0;
        while(SDL_PollEvent(&e) && index < 255) {
            inp->events[index++] = e;
        }
        inp->count = index;
    }
}

INPUT_EXPORT void titus_initialize(titus_application_context* ctx) {
    ecs_entity_t comp = TITUS_REGISTER_COMPONENT_NAMED(ctx->ecs, "core", "input", core_frame_input);
    if(0 == comp) {
        printf("Error registering component\n");
    }

    ecs_set_id(ctx->ecs, comp, comp, sizeof(core_frame_input), &(core_frame_input){.count = 0, .events = {0}});

    ecs_system(ctx->ecs,
               {
                   .callback = core_input_poll_system,
                   .entity   = ecs_entity(ctx->ecs,
                                          {
                                              .name = "input_polling",
                                              .add  = ecs_ids(ecs_dependson(EcsPreUpdate)),
                                        }),
                   .query.terms =
                       {
                           {.first.name = "core:input"},
                       },
               });
}

INPUT_EXPORT void titus_deinitialize(titus_application_context* ctx) {}