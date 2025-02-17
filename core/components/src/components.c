#include "core/components.h"
// #include <titus/sdk.h>

// void check_component_registration(ecs_entity_t e) {
//     if(e == 0) {
//         printf("Error creating component");
//     }
// }

// ECS_COMPONENT_DECLARE(core_frame_input);
// ECS_COMPONENT_DECLARE(core_render_material);

// extern void inputImport(ecs_world_t*);

// void coreImport(ecs_world_t* ecs) {

//     ECS_MODULE(ecs, core);

//     ECS_COMPONENT_DEFINE(ecs, core_frame_input);
//     ECS_COMPONENT_DEFINE(ecs, core_render_material);

//     ECS_IMPORT(ecs, input);
// }

// ecs_entity_t get_component_id() {
//     return ecs_id(core_frame_input);
// }

// CORE_EXPORT void titus_initialize(titus_application_context* ctx) {
//     titus_log_info("Registering core components");

//     ECS_IMPORT(ctx->ecs, core);

//     titus_log_info("Core components registered");
// }