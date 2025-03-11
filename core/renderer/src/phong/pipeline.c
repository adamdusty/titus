#include "phong/pipeline.h"

#include "gpu.h"
#include <titus/sdk.h>

SDL_GPUGraphicsPipeline* phong_create_pipeline(SDL_GPUDevice* device,
                                               SDL_GPUShader* vertex,
                                               SDL_GPUShader* fragment,
                                               SDL_GPUTextureFormat target_format) {

    SDL_GPUGraphicsPipelineCreateInfo pipeline_ci = {
        .vertex_shader   = vertex,
        .fragment_shader = fragment,
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
                        {.format = target_format},
                    },
                .num_color_targets        = 1,
                .depth_stencil_format     = SDL_GPU_TEXTUREFORMAT_D16_UNORM,
                .has_depth_stencil_target = true,
            },
    };

    SDL_GPUGraphicsPipeline* pipeline = SDL_CreateGPUGraphicsPipeline(device, &pipeline_ci);
    if(NULL == pipeline) {
        titus_log_error("Failed to create pipeline: %s", SDL_GetError());
    }

    return pipeline;
}