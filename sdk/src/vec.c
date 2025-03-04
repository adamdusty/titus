#include "titus/linalg/vec.h"

#include <math.h>

vec2f titus_vec2_add(const vec2f* lhs, const vec2f* rhs) {
    return (vec2f){
        .x = lhs->x + rhs->x,
        .y = lhs->y + rhs->y,
    };
}

vec3f titus_vec3_add(const vec3f* lhs, const vec3f* rhs) {
    return (vec3f){
        .x = lhs->x + rhs->x,
        .y = lhs->y + rhs->y,
        .z = lhs->z + rhs->z,
    };
}

vec4f titus_vec4_add(const vec4f* lhs, const vec4f* rhs) {
    return (vec4f){
        .w = lhs->w + rhs->w,
        .x = lhs->x + rhs->x,
        .y = lhs->y + rhs->y,
        .z = lhs->z + rhs->z,
    };
}

vec2f titus_vec2_sub(const vec2f* lhs, const vec2f* rhs) {
    return (vec2f){
        .x = lhs->x - rhs->x,
        .y = lhs->y - rhs->y,
    };
}

vec3f titus_vec3_sub(const vec3f* lhs, const vec3f* rhs) {
    return (vec3f){
        .x = lhs->x - rhs->x,
        .y = lhs->y - rhs->y,
        .z = lhs->z - rhs->z,
    };
}

vec4f titus_vec4_sub(const vec4f* lhs, const vec4f* rhs) {
    return (vec4f){
        .w = lhs->w - rhs->w,
        .x = lhs->x - rhs->x,
        .y = lhs->y - rhs->y,
        .z = lhs->z - rhs->z,
    };
}

vec2f titus_vec2_mul(const vec2f* lhs, const vec2f* rhs) {
    return (vec2f){
        .x = lhs->x * rhs->x,
        .y = lhs->y * rhs->y,
    };
}

vec3f titus_vec3_mul(const vec3f* lhs, const vec3f* rhs) {
    return (vec3f){
        .x = lhs->x * rhs->x,
        .y = lhs->y * rhs->y,
        .z = lhs->z * rhs->z,
    };
}

vec4f titus_vec4_mul(const vec4f* lhs, const vec4f* rhs) {
    return (vec4f){
        .w = lhs->w * rhs->w,
        .x = lhs->x * rhs->x,
        .y = lhs->y * rhs->y,
        .z = lhs->z * rhs->z,
    };
}

vec2f titus_vec2_div(const vec2f* lhs, const vec2f* rhs) {
    return (vec2f){
        .x = lhs->x / rhs->x,
        .y = lhs->y / rhs->y,
    };
}

vec3f titus_vec3_div(const vec3f* lhs, const vec3f* rhs) {
    return (vec3f){
        .x = lhs->x / rhs->x,
        .y = lhs->y / rhs->y,
        .z = lhs->z / rhs->z,
    };
}

vec4f titus_vec4_div(const vec4f* lhs, const vec4f* rhs) {
    return (vec4f){
        .w = lhs->w / rhs->w,
        .x = lhs->x / rhs->x,
        .y = lhs->y / rhs->y,
        .z = lhs->z / rhs->z,
    };
}

float titus_vec2_dot(const vec2f* lhs, const vec2f* rhs) {
    return lhs->x * rhs->x + lhs->y * rhs->y;
}

float titus_vec3_dot(const vec3f* lhs, const vec3f* rhs) {
    return lhs->x * rhs->x + lhs->y * rhs->y + lhs->z * rhs->z;
}

float titus_vec4_dot(const vec4f* lhs, const vec4f* rhs) {
    return lhs->w * rhs->w + lhs->x * rhs->x + lhs->y * rhs->y + lhs->z * rhs->z;
}

TITUS_EXPORT vec2f titus_vec2_norm(const vec2f* vec) {}

TITUS_EXPORT vec3f titus_vec3_norm(const vec3f* vec) {

    // Calculate the magnitude of the vector
    float magnitude = sqrtf(vec->x * vec->x + vec->y * vec->y + vec->z * vec->z);

    vec3f res = {0};
    if(magnitude > 0.0f) {
        res.x /= magnitude;
        res.y /= magnitude;
        res.z /= magnitude;
    }

    return res;
}

TITUS_EXPORT vec4f titus_vec4_norm(const vec4f* vec) {}

TITUS_EXPORT vec3f titus_vec3_cross(const vec3f* lhs, const vec3f* rhs) {}
