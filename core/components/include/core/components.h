#pragma once

#include <SDL3/SDL.h>
#include <stdint.h>
#include <titus/sdk.h>

typedef struct core_frame_input {
    size_t count;
    SDL_Event events[255];
} core_frame_input;
