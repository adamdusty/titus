#include "titus/linalg/mat.h"

#include <math.h>

mat4f titus_mat_identity() {
    return (mat4f){
        .w = {1.0f, 0.0f, 0.0f, 0.0f},
        .x = {0.0f, 1.0f, 0.0f, 0.0f},
        .y = {0.0f, 0.0f, 1.0f, 0.0f},
        .z = {0.0f, 0.0f, 0.0f, 1.0f},
    };
}

mat4f titus_mat_add(const mat4f* lhs, const mat4f* rhs) {
    return (mat4f){
        .w = titus_vec4_add(&lhs->w, &rhs->w),
        .x = titus_vec4_add(&lhs->x, &rhs->x),
        .y = titus_vec4_add(&lhs->y, &rhs->y),
        .z = titus_vec4_add(&lhs->z, &rhs->z),
    };
}

mat4f titus_mat_sub(const mat4f* lhs, const mat4f* rhs) {
    return (mat4f){
        .w = titus_vec4_sub(&lhs->w, &rhs->w),
        .x = titus_vec4_sub(&lhs->x, &rhs->x),
        .y = titus_vec4_sub(&lhs->y, &rhs->y),
        .z = titus_vec4_sub(&lhs->z, &rhs->z),
    };
}

mat4f titus_mat_mul(const mat4f* lhs, const mat4f* rhs) {
    mat4f result = {0};

    result.w.x = titus_vec4_dot(&lhs->w, &rhs->w);
    result.w.y = titus_vec4_dot(&lhs->w, &rhs->x);
    result.w.z = titus_vec4_dot(&lhs->w, &rhs->y);
    result.w.w = titus_vec4_dot(&lhs->w, &rhs->z);

    // Row 2 (x)
    result.x.x = titus_vec4_dot(&lhs->x, &rhs->w);
    result.x.y = titus_vec4_dot(&lhs->x, &rhs->x);
    result.x.z = titus_vec4_dot(&lhs->x, &rhs->y);
    result.x.w = titus_vec4_dot(&lhs->x, &rhs->z);

    // Row 3 (y)
    result.y.x = titus_vec4_dot(&lhs->y, &rhs->w);
    result.y.y = titus_vec4_dot(&lhs->y, &rhs->x);
    result.y.z = titus_vec4_dot(&lhs->y, &rhs->y);
    result.y.w = titus_vec4_dot(&lhs->y, &rhs->z);

    // Row 4 (z)
    result.z.x = titus_vec4_dot(&lhs->z, &rhs->w);
    result.z.y = titus_vec4_dot(&lhs->z, &rhs->x);
    result.z.z = titus_vec4_dot(&lhs->z, &rhs->y);
    result.z.w = titus_vec4_dot(&lhs->z, &rhs->z);

    return result;
}

mat4f titus_mat_translate(const mat4f* mat, const vec3f* translation) {
    mat4f res = *mat;

    res.w.x += translation->x;
    res.w.y += translation->y;
    res.w.z += translation->z;

    return res;
}

mat4f titus_mat_scale(const mat4f* mat, const vec3f* scale) {
    mat4f res = *mat;

    res.x.x *= scale->x;
    res.x.y *= scale->x;
    res.x.z *= scale->x;

    res.y.x *= scale->y;
    res.y.y *= scale->y;
    res.y.z *= scale->y;

    res.z.x *= scale->z;
    res.z.y *= scale->z;
    res.z.z *= scale->z;

    return res;
}

mat4f titus_mat_create_perspective_fov(float fieldOfView,
                                       float aspectRatio,
                                       float nearPlaneDistance,
                                       float farPlaneDistance) {
    float num = 1.0f / (tanf(fieldOfView * 0.5f));
    return (mat4f){
        {num / aspectRatio, 0, 0, 0},
        {0, num, 0, 0},
        {0, 0, farPlaneDistance / (nearPlaneDistance - farPlaneDistance), -1},
        {0, 0, (nearPlaneDistance * farPlaneDistance) / (nearPlaneDistance - farPlaneDistance), 0},
    };
}

mat4f titus_mat_look_at(const vec3f* pos, const vec3f* target, const vec3f* up) {
    vec3f target_to_pos = titus_vec3_norm((vec3f[]){titus_vec3_sub(pos, target)});
    vec3f perp          = titus_vec3_norm((vec3f[]){titus_vec3_cross(up, &target_to_pos)});
    vec3f new_up        = titus_vec3_cross(&target_to_pos, &perp);

    return (mat4f){
        {perp.x, new_up.x, target_to_pos.x, 0},
        {perp.y, new_up.y, target_to_pos.y, 0},
        {perp.z, new_up.z, target_to_pos.z, 0},
        {-titus_vec3_dot(&perp, pos), -titus_vec3_dot(&new_up, pos), -titus_vec3_dot(&target_to_pos, pos), 1},
    };
}
