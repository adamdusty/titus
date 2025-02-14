#include "core/components.h"
#include "core/export.h"
#include "core/renderer.h"
#include <titus/sdk.h>

void check_component_registration(ecs_entity_t e) {
    if(e == 0) {
        printf("Error creating component");
    }
}

ECS_COMPONENT_DECLARE(core_frame_input);
ECS_COMPONENT_DECLARE(core_render_material);

void coreImport(ecs_world_t* ecs) {
    ECS_COMPONENT_DEFINE(ecs, core_frame_input);
    ECS_COMPONENT_DEFINE(ecs, core_render_material);
}

// CORE_EXPORT void titus_initialize(titus_application_context* ctx) {
//     titus_log_info("Registering core components");

//     ECS_MODULE(ctx->ecs, core);
//     // ecs_entity_t core_input = TITUS_REGISTER_COMPONENT_NAMED(ctx->ecs, "core", "input", core_frame_input);
//     ECS_COMPONENT_DEFINE(ctx->ecs, core_frame_input);
//     ECS_COMPONENT_DEFINE(ctx->ecs, core_render_material);

//     titus_log_info("Core components registered");
// }