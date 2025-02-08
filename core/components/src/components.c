#include "core/components.h"
#include "core/export.h"
#include <titus/sdk.h>

void check_component_registration(ecs_entity_t e) {
    if(e == 0) {
        printf("Error creating component");
    }
}

CORE_EXPORT void titus_initialize(titus_application_context* ctx) {
    printf("Registering components\n");
    ecs_entity_t core_input = TITUS_REGISTER_COMPONENT_NAMED(ctx->ecs, "core", "input", core_frame_input);
}