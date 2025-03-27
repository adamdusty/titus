#pragma once

#include <SDL3/SDL.h>
#include <flecs.h>

typedef struct TitusApplicationContext {
    SDL_Window* window;
    ecs_world_t* ecs;
    // struct TitusModule* modules;
} TitusApplicationContext;
