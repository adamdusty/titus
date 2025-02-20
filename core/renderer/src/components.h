#pragma once

#include <SDL3/SDL.h>
#include <flecs.h>

// typedef struct core_render_pipeline {
//     SDL_GPUGraphicsPipeline* pipeline;
// } core_render_pipeline;

typedef struct vertex_position_color {
    float x;
    float y;
    float z;
    float w;
    float r;
    float g;
    float b;
    float a;
} vertex_position_color;

typedef struct render_mesh {
    vertex_position_color vertices[3];
} render_mesh;

extern ECS_COMPONENT_DECLARE(core_render_pipeline);
extern ECS_COMPONENT_DECLARE(render_mesh);
