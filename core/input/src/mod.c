#include "core/export.h"
#include <SDL3/SDL.h>
#include <core/components.h>
#include <flecs.h>
#include <titus/sdk.h>

void core_input_poll_system(ecs_iter_t* it);
void process_input(ecs_iter_t* it);

CORE_EXPORT void titus_initialize(titus_application_context* ctx) {
    ecs_entity_t comp = ecs_lookup(ctx->ecs, "core:input");

    // Attach input component to world as singleton
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

    ecs_system(ctx->ecs,
               {
                   .callback = process_input,
                   .entity   = ecs_entity(ctx->ecs,
                                          {
                                              .name = "input_processing",
                                              .add  = ecs_ids(ecs_dependson(EcsOnUpdate)),
                                        }),
                   .query.terms =
                       {
                           {.first.name = "core:input"},
                       },
               });
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
