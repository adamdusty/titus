#pragma once

#include <SDL3/SDL.h>

typedef struct CoreFrameInput {
    size_t count;
    SDL_Event events[255];
} CoreFrameInput;
