#pragma once

#include <core/export.h>
#include <flecs.h>
#include <stddef.h>
#include <stdint.h>
#include <titus/linalg/vec.h>

typedef struct Core_Camera {
    vec3f position;
    vec3f forward;
    vec3f up;
} Core_Camera;

typedef struct Core_VertexPosition {
    float x;
    float y;
    float z;
} Core_VertexPosition;

typedef struct Core_Mesh {
    Core_VertexPosition* vertices;
    size_t vertex_count;
    uint32_t* indices;
    size_t index_count;
} Core_Mesh;
