#pragma once

#include "titus/export.h"

typedef struct vec2f {
    float x;
    float y;
} vec2f;

typedef struct vec3f {
    float x;
    float y;
    float z;
} vec3f;

typedef struct vec4f {
    float x;
    float y;
    float z;
    float w;
} vec4f;

TITUS_EXPORT vec2f titus_vec2_add(const vec2f* lhs, const vec2f* rhs);
TITUS_EXPORT vec3f titus_vec3_add(const vec3f* lhs, const vec3f* rhs);
TITUS_EXPORT vec4f titus_vec4_add(const vec4f* lhs, const vec4f* rhs);

TITUS_EXPORT vec2f titus_vec2_sub(const vec2f* lhs, const vec2f* rhs);
TITUS_EXPORT vec3f titus_vec3_sub(const vec3f* lhs, const vec3f* rhs);
TITUS_EXPORT vec4f titus_vec4_sub(const vec4f* lhs, const vec4f* rhs);

TITUS_EXPORT vec2f titus_vec2_mul(const vec2f* lhs, const vec2f* rhs);
TITUS_EXPORT vec3f titus_vec3_mul(const vec3f* lhs, const vec3f* rhs);
TITUS_EXPORT vec4f titus_vec4_mul(const vec4f* lhs, const vec4f* rhs);

TITUS_EXPORT vec2f titus_vec2_div(const vec2f* lhs, const vec2f* rhs);
TITUS_EXPORT vec3f titus_vec3_div(const vec3f* lhs, const vec3f* rhs);
TITUS_EXPORT vec4f titus_vec4_div(const vec4f* lhs, const vec4f* rhs);

TITUS_EXPORT float titus_vec2_dot(const vec2f* lhs, const vec2f* rhs);
TITUS_EXPORT float titus_vec3_dot(const vec3f* lhs, const vec3f* rhs);
TITUS_EXPORT float titus_vec4_dot(const vec4f* lhs, const vec4f* rhs);
