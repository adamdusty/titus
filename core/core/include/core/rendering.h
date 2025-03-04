#pragma once

#include <core/export.h>
#include <flecs.h>
#include <stddef.h>
#include <stdint.h>
#include <titus/linalg/vec.h>

typedef struct CoreCamera {
    vec3f forward;
    vec3f up;
} CoreCamera;

typedef struct CoreVertexPosition {
    float x;
    float y;
    float z;
} CoreVertexPosition;

typedef struct CoreMesh {
    CoreVertexPosition* vertices;
    size_t vertex_count;
    uint32_t* indices;
    size_t index_count;
} CoreMesh;

/**
 * @brief Generates all the positions for a capsule shaped mesh. The returned pointer points to a section of malloced
 * memory and needs to be freed. The number of vec3f elements at the pointer is equal to (segments + 1) * (rings + 1).
 * Returns NULL on failure.
 *
 * @param radius
 * @param height
 * @param segments
 * @param rings
 * @param count[out] Set to the number of vec3f elements at the returned pointer
 * @return vec3f* Pointer to an array of positions.
 */
CORE_EXPORT CoreMesh core_create_capsule_mesh(float radius, float height, uint32_t slices, uint32_t stacks);
