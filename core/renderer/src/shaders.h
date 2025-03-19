#pragma once

#include "context.h"
#include <SDL3/SDL.h>
#include <flecs.h>
#include <titus/sds/sds.h>

extern sds core_renderer_resource_directory;

SDL_GPUGraphicsPipeline* create_default_pipeline(ecs_world_t* ecs, core_render_context* context);
