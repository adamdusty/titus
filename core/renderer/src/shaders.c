#include "shaders.h"

#include <titus/sdk.h>

SDL_GPUShaderCreateInfo shader_create_info_from_core_shader(const core_render_shader* s,
                                                            SDL_GPUShaderFormat fmt,
                                                            SDL_GPUShaderStage stage,
                                                            uint32_t samplers,
                                                            uint32_t storage_textures,
                                                            uint32_t storage_buffers,
                                                            uint32_t uniform_buffers) {
    return (SDL_GPUShaderCreateInfo){
        .code                 = s->code_bytes,
        .code_size            = s->code_size,
        .format               = fmt,
        .stage                = stage,
        .entrypoint           = "main",
        .num_samplers         = samplers,
        .num_storage_textures = storage_textures,
        .num_storage_buffers  = storage_buffers,
        .num_uniform_buffers  = uniform_buffers,
        .props                = 0,
    };
}

uint8_t* load_spirv(const char* path, size_t* size) {
    uint8_t* bytes = SDL_LoadFile(path, size);
    return bytes;
}

SDL_GPUGraphicsPipelineCreateInfo get_default_pipeline_create_info(ecs_world_t* ecs, core_render_context* context) {
    const char* vert_path = titus_get_asset_path(ecs, "core", "renderer", "shaders/vert.spv");
    size_t size           = 0;
    uint8_t* vert_bytes   = load_spirv(vert_path, &size);

    SDL_GPUGraphicsPipelineCreateInfo ci = {0};

    ci.vertex_shader      = NULL; // TODO: impl
    ci.fragment_shader    = NULL; // TODO: impl
    ci.vertex_input_state = (SDL_GPUVertexInputState){
        .num_vertex_buffers = 1,
        .vertex_buffer_descriptions =
            (SDL_GPUVertexBufferDescription[]){
                (SDL_GPUVertexBufferDescription){
                    .slot               = 0,
                    .pitch              = sizeof(float) * 4,
                    .input_rate         = SDL_GPU_VERTEXINPUTRATE_VERTEX,
                    .instance_step_rate = 0,
                },
            },
        .num_vertex_attributes = 1,
        .vertex_attributes =
            (SDL_GPUVertexAttribute[]){
                {.format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4, .location = 0, .offset = 0, .buffer_slot = 0},
            },
    };
    ci.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;

    ci.rasterizer_state           = (SDL_GPURasterizerState){0};
    ci.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_FILL;
    ci.rasterizer_state.cull_mode = SDL_GPU_CULLMODE_NONE;

    ci.multisample_state   = (SDL_GPUMultisampleState){0};
    ci.depth_stencil_state = (SDL_GPUDepthStencilState){0};

    ci.target_info = (SDL_GPUGraphicsPipelineTargetInfo){
        .num_color_targets = 1,
        .color_target_descriptions =
            (SDL_GPUColorTargetDescription[]){
                {.format = SDL_GetGPUSwapchainTextureFormat(context->device, context->window)},
            },
    };

    return ci;
}
