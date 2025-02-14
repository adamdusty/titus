#pragma once

#include "core/export.h"
#include "core/input.h"
#include "core/renderer.h"
#include <flecs.h>


extern ECS_COMPONENT_DECLARE(core_render_material);
extern ECS_COMPONENT_DECLARE(core_frame_input);

CORE_EXPORT void coreImport(ecs_world_t* ecs);