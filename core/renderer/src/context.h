#pragma once

#include <SDL3/SDL.h>

typedef struct core_render_context {
    SDL_Window* window;
    SDL_GPUDevice* device;
} core_render_context;