#pragma once

#include <flecs.h>
#include <stdint.h>

typedef struct core_render_shader {
    size_t code_size;
    uint8_t* code_bytes;
} core_render_shader;

typedef struct core_render_material {
} core_render_material;
