#pragma once

#include <SDL3/SDL.h>
#include <core/components.h>
#include <flecs.h>

typedef struct CoreMeshRenderInfo {
    SDL_GPUBuffer* vertex_buffer;
    SDL_GPUBuffer* index_buffer;
} CoreMeshRenderInfo;