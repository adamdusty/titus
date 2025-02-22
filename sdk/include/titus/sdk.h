#pragma once

#include "titus/assets/assets.h"
#include "titus/context/context.h"
#include "titus/ecs/ecs.h"
#include "titus/linalg/vec.h"
#include "titus/log/log.h"
#include <SDL3/SDL.h>
#include <flecs.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

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
TITUS_EXPORT vec3f*
core_capsule_mesh_positions(float radius, float height, size_t segments, size_t rings, size_t* count);