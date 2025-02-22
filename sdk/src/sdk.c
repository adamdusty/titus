#include "titus/sdk.h"

#include <math.h>
#include <stdlib.h>

vec3f* core_capsule_mesh_positions(float radius, float height, size_t segments, size_t rings, size_t* count) {

    size_t vertex_count = (rings + 1) * (segments + 1);

    vec3f* positions = (vec3f*)malloc(vertex_count * sizeof(vec3f));
    if(!positions) {
        *count = 0;
        return NULL;
    }

    *count = vertex_count;

    // Generate vertices
    float phi_step    = M_PI / (float)rings;
    float theta_step  = 2.0f * M_PI / (float)segments;
    float half_height = height * 0.5f;

    int vertexIndex = 0;
    for(size_t i = 0; i <= rings; ++i) {
        float phi        = (float)i * phi_step;
        float y          = cosf(phi) * radius;
        float ringRadius = sinf(phi) * radius;

        for(size_t j = 0; j <= segments; ++j) {
            float theta = (float)j * theta_step;
            float x     = cosf(theta) * ringRadius;
            float z     = sinf(theta) * ringRadius;

            // Adjust y for the hemispherical caps
            if(i <= rings / 2) {
                y -= half_height;
            } else {
                y += half_height;
            }

            // Store the position
            positions[vertexIndex].x = x;
            positions[vertexIndex].y = y;
            positions[vertexIndex].z = z;
            vertexIndex++;
        }
    }

    return positions;
}
