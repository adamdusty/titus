#pragma once

#include <SDL3/SDL.h>
#include <flecs.h>

typedef struct titus_application_context {
    SDL_Window* window;
    ecs_world_t* ecs;
} titus_application_context;
