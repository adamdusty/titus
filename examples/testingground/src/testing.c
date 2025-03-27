#include <titus/sdk.h>

#include "testing/export.h"

#include <core/core.h>
#include <flecs.h>
#include <math.h>

void testing_system(ecs_iter_t* it);
static float t = 0;

TG_EXPORT void titus_initialize(const TitusApplicationContext* ctx, sds module_root_directory) {
    titus_log_info("Hello from testing");

    ECS_SYSTEM(ctx->ecs, testing_system, EcsOnUpdate, core.CoreCamera, core.CorePosition);

    ecs_entity_t camera_entity = ecs_entity(ctx->ecs, {.name = "testing.camera"});
    ecs_set(ctx->ecs, camera_entity, CoreCamera, {.up = {0, 1.0f, 0}, .forward = {0, 0, 1.0f}});
    ecs_set(ctx->ecs, camera_entity, CorePosition, {0.0f, 0.0f, -10.0f});

    ecs_entity_t cube   = ecs_entity(ctx->ecs, {.name = "testing.cube"});
    CoreMesh* cube_mesh = ecs_ensure(ctx->ecs, cube, CoreMesh);
    *cube_mesh          = core_create_cube_mesh(0.5f, 0.5f, 0.5f);
    ecs_modified(ctx->ecs, cube, CoreMesh);
}

void testing_system(ecs_iter_t* it) {
    CorePosition* pos = ecs_field(it, CorePosition, 1);
    t += it->delta_time;

    for(int i = 0; i < it->count; i++) {
        pos[i][0] = sinf(t * 1.0f) * 5.0f;
        pos[i][1] = sinf(t * 1.0f) * 5.0f;
    }
}
