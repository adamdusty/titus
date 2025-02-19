#include "core/core.h"

#include "core/export.h"
#include "core/input.h"
#include "core/renderer.h"
#include <titus/sdk.h>

ECS_COMPONENT_DECLARE(core_render_material);
ECS_COMPONENT_DECLARE(core_frame_input);

CORE_EXPORT void coreImport(ecs_world_t* ecs) {
    titus_log_info("Importing the core module");

    ECS_MODULE(ecs, core);
    titus_log_info("Module registered");

    ECS_COMPONENT_DEFINE(ecs, core_frame_input);
    ECS_COMPONENT_DEFINE(ecs, core_render_material);

    titus_log_info("Core module imported");
}

CORE_EXPORT void titus_initialize(titus_application_context* ctx) {
    titus_log_info("Initializing the core module");

    ECS_IMPORT(ctx->ecs, core);
}