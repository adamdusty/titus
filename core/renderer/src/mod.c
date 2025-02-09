#include "core/export.h"
#include <SDL3/SDL.h>
#include <titus/sdk.h>

typedef struct core_render_context {
    SDL_Window* window;
    SDL_GPUDevice* device;
} core_render_context;

CORE_EXPORT void titus_initialize(const titus_application_context* ctx) {
    core_render_context render_context = {0};
    SDL_Log("Initializing core:renderer module");

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

    render_context.window = ctx->window;
    render_context.device = gpu_device;

    ecs_entity_t ctx_comp = TITUS_REGISTER_COMPONENT_NAMED(ctx->ecs, "core", "render_context", core_render_context);

    ecs_set_id(ctx->ecs, ctx_comp, ctx_comp, sizeof(core_render_context), &render_context);
}

CORE_EXPORT void titus_deinitialize(titus_application_context* ctx) {
    ecs_entity_t ctx_comp   = ecs_lookup(ctx->ecs, "core:render_context");
    core_render_context* rc = ecs_get_mut_id(ctx->ecs, ctx_comp, ctx_comp);

    SDL_DestroyGPUDevice(rc->device);
}