#pragma once

#include <SDL3/SDL.h>

typedef struct Core_FrameInput {
    size_t count;
    SDL_Event events[255];
} Core_FrameInput;
