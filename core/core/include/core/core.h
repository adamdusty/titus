#pragma once

#include "core/export.h"
#include <flecs.h>
#include <titus/sdk.h>

// extern ECS_COMPONENT_DECLARE(core_render_material);
// extern ECS_COMPONENT_DECLARE(Core_FrameInput);

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
CORE_EXPORT vec3f*
core_capsule_mesh_positions(float radius, float height, size_t segments, size_t rings, size_t* count);