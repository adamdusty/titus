#include "core/core.h"

#include "core/export.h"
#include "core/input.h"
#include "core/rendering.h"
#include <titus/sdk.h>

ECS_COMPONENT_DECLARE(CoreFrameInput);
ECS_COMPONENT_DECLARE(CoreMesh);
ECS_COMPONENT_DECLARE(CoreCamera);

CORE_EXPORT void coreImport(ecs_world_t* ecs) {
    titus_log_info("Importing the core module");

    ECS_MODULE(ecs, core);

    ECS_COMPONENT_DEFINE(ecs, CoreFrameInput);
    ECS_COMPONENT_DEFINE(ecs, CoreCamera);
    ECS_COMPONENT_DEFINE(ecs, CoreMesh);

    titus_log_info("Core module imported");
}

CORE_EXPORT void titus_initialize(titus_application_context* ctx) {
    titus_log_info("Initializing the core module");

    ECS_IMPORT(ctx->ecs, core);
}

CORE_EXPORT void titus_deinitialize(titus_application_context* /* ctx */) {
    titus_log_info("Deinitializing the core module");
}
