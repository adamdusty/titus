#include "core/core.h"

#include "core/components.h"
#include "core/export.h"
#include "core/input.h"
#include "core/rendering.h"
#include <titus/sdk.h>

int core_module_version = 42;

CORE_MODULE_EXPORT void coreImport(ecs_world_t* ecs) {
    titus_log_info("Importing the core module");

    ECS_MODULE(ecs, core);

    ECS_COMPONENT_DEFINE(ecs, CoreFrameInput);
    ECS_COMPONENT_DEFINE(ecs, CoreCamera);
    ECS_COMPONENT_DEFINE(ecs, CoreMesh);
    ECS_COMPONENT_DEFINE(ecs, CorePosition);
    ECS_COMPONENT_DEFINE(ecs, CoreScale);
    ECS_COMPONENT_DEFINE(ecs, CoreRotation);

    titus_log_info("Core module imported");
}

CORE_MODULE_EXPORT void titus_initialize(titus_application_context* ctx) {
    titus_log_info("Initializing the core module");

    ECS_IMPORT(ctx->ecs, core);
}

CORE_MODULE_EXPORT void titus_deinitialize(titus_application_context* /* ctx */) {
    titus_log_info("Deinitializing the core module");
}
