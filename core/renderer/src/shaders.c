#include "shaders.h"

#include "gpu.h"
#include <core/core.h>
#include <titus/sdk.h>

SDL_GPUGraphicsPipeline* create_default_pipeline(ecs_world_t* ecs, core_render_context* context) {
    sds vert_path = sdsdup(core_renderer_resource_directory);
    vert_path     = sdscat(vert_path, "shaders/phong.vert.spv");
    titus_log_debug("vert_path: %s", vert_path);
    size_t vsize        = 0;
    uint8_t* vert_bytes = SDL_LoadFile(vert_path, &vsize);
    if(NULL == vert_bytes) {
        titus_log_error("Failed to load shader at %s: %s", vert_path, SDL_GetError());
    }

    SDL_GPUShaderCreateInfo vert_ci = {0};
    vert_ci.code                    = vert_bytes;
    vert_ci.code_size               = vsize;
    vert_ci.format                  = SDL_GPU_SHADERFORMAT_SPIRV;
    vert_ci.entrypoint              = "main";
    vert_ci.stage                   = SDL_GPU_SHADERSTAGE_VERTEX;
    vert_ci.num_uniform_buffers     = 1;

    SDL_GPUShader* vert_shader = SDL_CreateGPUShader(context->device, &vert_ci);

    sds frag_path       = sdsdup(core_renderer_resource_directory);
    frag_path           = sdscat(frag_path, "shaders/phong.frag.spv");
    size_t fsize        = 0;
    uint8_t* frag_bytes = load_spirv(frag_path, &fsize);
    if(NULL == frag_bytes) {
        titus_log_error("Failed to load shader at %s: %s", vert_path, SDL_GetError());
    }

    SDL_GPUShaderCreateInfo frag_ci = {0};
    frag_ci.code                    = frag_bytes;
    frag_ci.code_size               = fsize;
    frag_ci.format                  = SDL_GPU_SHADERFORMAT_SPIRV;
    frag_ci.entrypoint              = "main";
    frag_ci.stage                   = SDL_GPU_SHADERSTAGE_FRAGMENT;

    SDL_GPUShader* frag_shader = SDL_CreateGPUShader(context->device, &frag_ci);

    SDL_GPUGraphicsPipelineCreateInfo pipeline_ci = {
        .vertex_shader   = vert_shader,
        .fragment_shader = frag_shader,
        .vertex_input_state =
            {
                .vertex_buffer_descriptions =
                    (SDL_GPUVertexBufferDescription[]){
                        {
                            .slot               = 0,
                            .pitch              = sizeof(CoreVertexPositionNormal),
                            .input_rate         = SDL_GPU_VERTEXINPUTRATE_VERTEX,
                            .instance_step_rate = 0,
                        },
                    },
                .num_vertex_buffers = 1,
                .vertex_attributes =
                    (SDL_GPUVertexAttribute[]){
                        {.format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, .location = 0, .offset = 0, .buffer_slot = 0},
                        {.format      = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
                         .location    = 1,
                         .offset      = sizeof(float) * 3,
                         .buffer_slot = 0},
                    },
                .num_vertex_attributes = 2,
            },
        .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
        .rasterizer_state =
            {
                .fill_mode  = SDL_GPU_FILLMODE_FILL,
                .cull_mode  = SDL_GPU_CULLMODE_NONE,
                .front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE,
            },
        .depth_stencil_state =
            {
                .compare_op         = SDL_GPU_COMPAREOP_LESS,
                .enable_depth_test  = true,
                .enable_depth_write = true,
            },
        .target_info =
            {
                .color_target_descriptions =
                    (SDL_GPUColorTargetDescription[]){
                        {.format = SDL_GetGPUSwapchainTextureFormat(context->device, context->window)},
                    },
                .num_color_targets        = 1,
                .depth_stencil_format     = SDL_GPU_TEXTUREFORMAT_D16_UNORM,
                .has_depth_stencil_target = true,
            },
    };

    SDL_GPUGraphicsPipeline* pipeline = SDL_CreateGPUGraphicsPipeline(context->device, &pipeline_ci);
    if(NULL == pipeline) {
        titus_log_error("Failed to create pipeline: %s", SDL_GetError());
    }

    SDL_ReleaseGPUShader(context->device, vert_shader);
    SDL_ReleaseGPUShader(context->device, frag_shader);

    return pipeline;
}
