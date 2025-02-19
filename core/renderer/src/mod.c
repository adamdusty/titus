#include "core/export.h"
#include "core/renderer.h"
#include <SDL3/SDL.h>
#include <titus/sdk.h>

static ecs_entity_t core_render_context_id = 0;

typedef struct core_render_context {
    SDL_Window* window;
    SDL_GPUDevice* device;
} core_render_context;

SDL_GPUShaderCreateInfo shader_create_info_from_core_shader(const core_render_shader* s,
                                                            SDL_GPUShaderFormat fmt,
                                                            SDL_GPUShaderStage stage,
                                                            uint32_t samplers,
                                                            uint32_t storage_textures,
                                                            uint32_t storage_buffers,
                                                            uint32_t uniform_buffers);
void render_frame(ecs_iter_t* it);
uint8_t* load_spirv(const char* path, size_t* size);

ECS_COMPONENT_DECLARE(core_render_context);
ECS_SYSTEM_DECLARE(render_frame);

void rendererImport(ecs_world_t* ecs) {

    ECS_MODULE(ecs, CoreRenderer);

    ECS_COMPONENT_DEFINE(ecs, core_render_context);

    ECS_SYSTEM_DEFINE(ecs, render_frame, EcsPostUpdate, core.renderer.core_render_context($));
}

void render_frame(ecs_iter_t* it);

CORE_EXPORT void titus_initialize(const titus_application_context* ctx) {
    titus_log_info("Initializing core.renderer module");

    ecs_entity_t m = ecs_import(ctx->ecs, rendererImport, "core.renderer");
    if(0 == m) {
        titus_log_error("Failed to load module");
        return;
    }

    core_render_context* render_context = ecs_singleton_ensure(ctx->ecs, core_render_context);

    SDL_GPUDevice* gpu_device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, "vulkan");
    if(NULL == gpu_device) {
        titus_log_error("Error initializing GPU device from SDL: %s", SDL_GetError());
        return;
    }

    if(NULL == ctx->window) {
        titus_log_error("Application window is null");
        return;
    }

    if(!SDL_ClaimWindowForGPUDevice(gpu_device, ctx->window)) {
        titus_log_error("Failed to claim GPU device for window: %s", SDL_GetError());
        return;
    }

    render_context->window = ctx->window;
    render_context->device = gpu_device;

    SDL_SetGPUSwapchainParameters(
        render_context->device, render_context->window, SDL_GPU_SWAPCHAINCOMPOSITION_SDR, SDL_GPU_PRESENTMODE_VSYNC);
}

CORE_EXPORT void titus_deinitialize(titus_application_context* ctx) {
    titus_log_info("Deinitializing core.renderer");

    const core_render_context* rend = ecs_singleton_get(ctx->ecs, core_render_context);
    SDL_DestroyGPUDevice(rend->device);
}

uint8_t* load_spirv(const char* path, size_t* size) {
    uint8_t* bytes = SDL_LoadFile(path, size);
    if(NULL == bytes) {
        titus_log_error("Failed to load from path [%s]: %s", path, SDL_GetError());
    }

    return bytes;
}

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

void render_frame(ecs_iter_t* it) {
    core_render_context* ctx = ecs_field(it, core_render_context, 0);

    SDL_Window* win    = ctx->window;
    SDL_GPUDevice* dev = ctx->device;

    SDL_GPUCommandBuffer* cmdbuf = SDL_AcquireGPUCommandBuffer(dev);
    if(cmdbuf == NULL) {
        titus_log_error("AcquireGPUCommandBuffer failed: %s", SDL_GetError());
        return;
    }

    SDL_GPUTexture* swapchainTexture;
    if(!SDL_WaitAndAcquireGPUSwapchainTexture(cmdbuf, win, &swapchainTexture, NULL, NULL)) {
        titus_log_error("WaitAndAcquireGPUSwapchainTexture failed: %s", SDL_GetError());
        return;
    }

    if(swapchainTexture != NULL) {
        SDL_GPUColorTargetInfo colorTargetInfo = {0};
        colorTargetInfo.texture                = swapchainTexture;
        colorTargetInfo.clear_color            = (SDL_FColor){0.3f, 0.4f, 0.5f, 1.0f};
        colorTargetInfo.load_op                = SDL_GPU_LOADOP_CLEAR;
        colorTargetInfo.store_op               = SDL_GPU_STOREOP_DONT_CARE;

        SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(cmdbuf, &colorTargetInfo, 1, NULL);
        SDL_EndGPURenderPass(renderPass);
    }

    SDL_SubmitGPUCommandBuffer(cmdbuf);

    return;
}