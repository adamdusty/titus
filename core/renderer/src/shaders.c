#include "shaders.h"

#include "components.h"
#include <titus/sdk.h>

uint8_t* load_spirv(const char* path, size_t* size) {
    uint8_t* bytes = SDL_LoadFile(path, size);
    return bytes;
}

SDL_GPUGraphicsPipeline* create_default_pipeline(ecs_world_t* ecs, core_render_context* context) {
    const char* vert_path = titus_get_asset_path(ecs, "core", "renderer", "shaders/vert.spv");
    size_t vsize          = 0;
    uint8_t* vert_bytes   = load_spirv(vert_path, &vsize);

    SDL_GPUShaderCreateInfo vert_ci = {0};
    vert_ci.code                    = vert_bytes;
    vert_ci.code_size               = vsize;
    vert_ci.format                  = SDL_GPU_SHADERFORMAT_SPIRV;
    vert_ci.entrypoint              = "main";
    vert_ci.stage                   = SDL_GPU_SHADERSTAGE_VERTEX;

    SDL_GPUShader* vert_shader = SDL_CreateGPUShader(context->device, &vert_ci);

    const char* frag_path = titus_get_asset_path(ecs, "core", "renderer", "shaders/frag.spv");
    size_t fsize          = 0;
    uint8_t* frag_bytes   = load_spirv(frag_path, &fsize);

    SDL_GPUShaderCreateInfo frag_ci = {0};
    frag_ci.code                    = frag_bytes;
    frag_ci.code_size               = fsize;
    frag_ci.format                  = SDL_GPU_SHADERFORMAT_SPIRV;
    frag_ci.entrypoint              = "main";
    frag_ci.stage                   = SDL_GPU_SHADERSTAGE_FRAGMENT;

    SDL_GPUShader* frag_shader = SDL_CreateGPUShader(context->device, &frag_ci);

    SDL_GPUGraphicsPipelineCreateInfo ci = {0};

    ci.vertex_shader      = vert_shader;
    ci.fragment_shader    = frag_shader;
    ci.vertex_input_state = (SDL_GPUVertexInputState){
        .num_vertex_buffers = 1,
        .vertex_buffer_descriptions =
            (SDL_GPUVertexBufferDescription[]){
                (SDL_GPUVertexBufferDescription){
                    .slot               = 0,
                    .pitch              = sizeof(vertex_position_color),
                    .input_rate         = SDL_GPU_VERTEXINPUTRATE_VERTEX,
                    .instance_step_rate = 0,
                },
            },
        .num_vertex_attributes = 2,
        .vertex_attributes =
            (SDL_GPUVertexAttribute[]){
                {.format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4, .location = 0, .offset = 0, .buffer_slot = 0},
                {.format      = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4,
                 .location    = 1,
                 .offset      = sizeof(float) * 4,
                 .buffer_slot = 0},
            },
    };
    ci.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLESTRIP;

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

    SDL_GPUGraphicsPipeline* pipeline = SDL_CreateGPUGraphicsPipeline(context->device, &ci);
    if(NULL == pipeline) {
        titus_log_error("Failed to create pipeline: %s", SDL_GetError());
    }

    return pipeline;
}
