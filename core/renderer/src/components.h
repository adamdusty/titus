#pragma once

#include <SDL3/SDL.h>
#include <flecs.h>

// typedef struct core_render_pipeline {
//     SDL_GPUGraphicsPipeline* pipeline;
// } core_render_pipeline;

typedef struct vertex_position {
    float x;
    float y;
    float z;
    float w;
} vertex_position;

typedef struct render_mesh {
    vertex_position vertices[3];
} render_mesh;

extern ECS_COMPONENT_DECLARE(core_render_pipeline);
extern ECS_COMPONENT_DECLARE(render_mesh);
