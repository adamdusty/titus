#pragma once

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

mat4f titus_mat_add(const mat4f* lhs, const mat4f* rhs);
mat4f titus_mat_sub(const mat4f* lhs, const mat4f* rhs);
mat4f titus_mat_mul(const mat4f* lhs, const mat4f* rhs);
mat4f titus_mat_translate(const mat4f* mat, vec3f translation);
mat4f titus_mat_scale(const mat4f* mat, vec3f translation);
