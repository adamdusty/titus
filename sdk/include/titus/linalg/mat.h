#pragma once

#include "titus/export.h"
#include "titus/linalg/vec.h"

/**
 * @brief Row-major 4x4 matrix of floats
 *
 */
typedef struct mat4f {
    vec4f w;
    vec4f x;
    vec4f y;
    vec4f z;
} mat4f;

/*PERF:
 * Currently, the matrix operations copy the matrix and return a new one.
 * Potential performance gain by just modifying the matrix passed in.
 */

TITUS_EXPORT mat4f titus_mat_identity();
TITUS_EXPORT mat4f titus_mat_add(const mat4f* lhs, const mat4f* rhs);
TITUS_EXPORT mat4f titus_mat_sub(const mat4f* lhs, const mat4f* rhs);
TITUS_EXPORT mat4f titus_mat_mul(const mat4f* lhs, const mat4f* rhs);
TITUS_EXPORT mat4f titus_mat_translate(const mat4f* mat, const vec3f* translation);
TITUS_EXPORT mat4f titus_mat_scale(const mat4f* mat, const vec3f* scale);

TITUS_EXPORT mat4f titus_mat_create_perspective_fov(float fieldOfView,
                                                    float aspectRatio,
                                                    float nearPlaneDistance,
                                                    float farPlaneDistance);
