#pragma once

#include <SDL3/SDL.h>
#include <flecs.h>
#include <stdint.h>

typedef struct core_render_shader {
    size_t code_size;
    uint8_t* code_bytes;
} core_render_shader;

typedef struct core_render_material {
    SDL_GPUGraphicsPipelineCreateInfo pipeline_create_info;
} core_render_material;
