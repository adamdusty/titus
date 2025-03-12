#pragma once

#include <SDL3/SDL.h>

typedef struct CoreRenderContext {
    SDL_Window* window;
    SDL_GPUDevice* device;
} CoreRenderContext;