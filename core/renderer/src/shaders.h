#pragma once

#include "context.h"
#include "core/renderer.h"
#include <SDL3/SDL.h>
#include <flecs.h>

uint8_t* load_spirv(const char* path, size_t* size);

SDL_GPUGraphicsPipelineCreateInfo get_default_pipeline_create_info(ecs_world_t* ecs, core_render_context* context);
SDL_GPUShaderCreateInfo shader_create_info_from_core_shader(const core_render_shader* s,
                                                            SDL_GPUShaderFormat fmt,
                                                            SDL_GPUShaderStage stage,
                                                            uint32_t samplers,
                                                            uint32_t storage_textures,
                                                            uint32_t storage_buffers,
                                                            uint32_t uniform_buffers);