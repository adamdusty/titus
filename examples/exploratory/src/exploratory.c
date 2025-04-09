#include <titus/sdk.h>

#include "exploratory/export.h"

#include <core/core.h>
#include <flecs.h>
#include <math.h>

static float t = 0;

ecs_entity_t create_camera(ecs_world_t* ecs);
ecs_entity_t create_example_entity(ecs_world_t* ecs);

void test_system(ecs_iter_t* it);

EXPLORATORY_EXPORT void titus_initialize(const TitusApplicationContext* ctx, sds module_root_directory) {
    ecs_entity_t camera = create_camera(ctx->ecs);
    ecs_entity_t e      = create_example_entity(ctx->ecs);

    ECS_SYSTEM(ctx->ecs, test_system, EcsOnUpdate, core.CoreCamera, core.CorePosition);
}

ecs_entity_t create_camera(ecs_world_t* ecs) {
    ecs_entity_t camera_entity = ecs_new(ecs);

    ecs_set(ecs, camera_entity, CoreCamera, {.up = {0, 1.0f, 0}, .forward = {0, 0, 1.0f}});
    ecs_set(ecs, camera_entity, CorePosition, {5.0f, 0.0f, -10.0f});

    return camera_entity;
}

ecs_entity_t create_example_entity(ecs_world_t* ecs) {
    ecs_entity_t e = ecs_new(ecs);

    ecs_set_id(ecs, e, ecs_id(CoreMesh), sizeof(CoreMesh), (CoreMesh[]){core_create_cube_mesh(1.0f, 1.0f, 1.0f)});
    ecs_set(ecs, e, CorePosition, {0, 0, 0});

    return e;
}

void test_system(ecs_iter_t* it) {
    CoreCamera* cam   = ecs_field(it, CoreCamera, 0);
    CorePosition* pos = ecs_field(it, CorePosition, 1);

    t += it->delta_time;

    for(int i = 0; i < it->count; i++) {
        pos[i][0] = sinf(t) * 5.0f;
        pos[i][1] = sinf(t) * 5.0f;
    }
}

/*
    titus_log_info("Hello from testing");

    ECS_SYSTEM(ctx->ecs, testing_system, EcsOnUpdate, core.CoreCamera, core.CorePosition);

    ecs_entity_t camera_entity = ecs_entity(ctx->ecs, {.name = "testing.camera"});
    ecs_set(ctx->ecs, camera_entity, CoreCamera, {.up = {0, 1.0f, 0}, .forward = {0, 0, 1.0f}});
    ecs_set(ctx->ecs, camera_entity, CorePosition, {0.0f, 0.0f, -10.0f});

    ecs_entity_t cube   = ecs_entity(ctx->ecs, {.name = "testing.cube"});
    CoreMesh* cube_mesh = ecs_ensure(ctx->ecs, cube, CoreMesh);
    *cube_mesh          = core_create_cube_mesh(0.5f, 0.5f, 0.5f);
    ecs_modified(ctx->ecs, cube, CoreMesh);

*/