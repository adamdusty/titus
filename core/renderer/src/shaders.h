#pragma once

#include "context.h"
#include <SDL3/SDL.h>
#include <flecs.h>

uint8_t* load_spirv(const char* path, size_t* size);

SDL_GPUGraphicsPipeline* create_default_pipeline(ecs_world_t* ecs, core_render_context* context);
