#include "core/rendering.h"

#include <SDL3/SDL.h>
#include <math.h>
#include <titus/sdk.h>

vec3f* core_capsule_mesh_positions(float radius, float height, size_t segments, size_t rings, size_t* count) {
    if(radius <= 0 || height < 0 || segments < 3 || rings < 2) {
        *count = 0;
        return NULL; // Invalid parameters
    }

    // Calculate the total number of vertices
    // - Top hemisphere: rings * segments
    // - Cylinder: 2 * segments (top and bottom rings)
    // - Bottom hemisphere: rings * segments
    *count = (2 * rings + 2) * segments;

    // Allocate memory for all vertices
    vec3f* vertices = (vec3f*)SDL_malloc(*count * sizeof(vec3f));
    if(!vertices) {
        *count = 0;
        return NULL; // Memory allocation failed
    }

    size_t vertex_index = 0;
    float const R       = 1.0f / (float)(rings - 1);
    float const S       = 1.0f / (float)(segments - 1);

    // Generate top hemisphere vertices
    for(size_t r = 0; r < rings; r++) {
        float const phi = M_PI * 0.5f * (1.0f - r * R); // π/2 to 0 (top to equator)
        for(size_t s = 0; s < segments; s++) {
            float const theta = 2.0f * M_PI * s * S;

            vertices[vertex_index].x = radius * cosf(phi) * cosf(theta);
            vertices[vertex_index].y = radius * sinf(phi) + height * 0.5f;
            vertices[vertex_index].z = radius * cosf(phi) * sinf(theta);
            vertex_index++;
        }
    }

    // Generate cylinder vertices (top ring)
    float y_top = height * 0.5f;
    for(size_t s = 0; s < segments; s++) {
        float const theta = 2.0f * M_PI * s * S;

        vertices[vertex_index].x = radius * cosf(theta);
        vertices[vertex_index].y = y_top;
        vertices[vertex_index].z = radius * sinf(theta);
        vertex_index++;
    }

    // Generate cylinder vertices (bottom ring)
    float y_bottom = -height * 0.5f;
    for(size_t s = 0; s < segments; s++) {
        float const theta = 2.0f * M_PI * s * S;

        vertices[vertex_index].x = radius * cosf(theta);
        vertices[vertex_index].y = y_bottom;
        vertices[vertex_index].z = radius * sinf(theta);
        vertex_index++;
    }

    // Generate bottom hemisphere vertices
    for(size_t r = 0; r < rings; r++) {
        float const phi = M_PI * 0.5f * r * R; // 0 to π/2 (equator to bottom)
        for(size_t s = 0; s < segments; s++) {
            float const theta = 2.0f * M_PI * s * S;

            vertices[vertex_index].x = radius * cosf(phi) * cosf(theta);
            vertices[vertex_index].y = -radius * sinf(phi) - height * 0.5f;
            vertices[vertex_index].z = radius * cosf(phi) * sinf(theta);
            vertex_index++;
        }
    }

    return vertices;
}
