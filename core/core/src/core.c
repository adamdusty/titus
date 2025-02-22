#include "core/core.h"

#include "core/export.h"
#include "core/input.h"
#include "core/rendering.h"
#include <titus/sdk.h>

// ECS_COMPONENT_DECLARE(core_render_material);
ECS_COMPONENT_DECLARE(Core_FrameInput);
ECS_COMPONENT_DECLARE(Core_Mesh);
ECS_COMPONENT_DECLARE(Core_Camera);

CORE_EXPORT void coreImport(ecs_world_t* ecs) {
    titus_log_info("Importing the core module");

    ECS_MODULE(ecs, core);

    ECS_COMPONENT_DEFINE(ecs, Core_FrameInput);
    ECS_COMPONENT_DEFINE(ecs, Core_Camera);
    ECS_COMPONENT_DEFINE(ecs, Core_Mesh);

    titus_log_info("Core module imported");
}

CORE_EXPORT void titus_initialize(titus_application_context* ctx) {
    titus_log_info("Initializing the core module");

    ECS_IMPORT(ctx->ecs, core);
}
