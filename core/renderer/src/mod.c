#include "core/export.h"
#include "core/renderer.h"
#include <SDL3/SDL.h>
#include <titus/sdk.h>

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
uint8_t* load_spirv(const char* path, size_t* size);

ECS_COMPONENT_DECLARE(core_render_context);

void rendererImport(ecs_world_t* ecs) {
    ecs_entity_t renderer_module = 0;
    ecs_component_desc_t desc    = {0};
    desc.entity                  = renderer_module;
    ecs_module_init(ecs, "core.renderer", &desc);
    ecs_set_scope(ecs, renderer_module);

    ECS_COMPONENT_DEFINE(ecs, core_render_context);
}

CORE_EXPORT void titus_initialize(const titus_application_context* ctx) {
    titus_log_info("Initializing core:renderer module");

    ecs_import(ctx->ecs, rendererImport, "core.renderer");

    ecs_entity_t main_render_context    = ecs_entity(ctx->ecs, {.name = "main_render_context"});
    core_render_context* render_context = ecs_ensure(ctx->ecs, main_render_context, core_render_context);

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

    char* resource_path = titus_get_asset_path(ctx->ecs, "core", "renderer", "shaders/vert.spv");
    size_t size;
    uint8_t* bytes = load_spirv(resource_path, &size);

    core_render_shader crs = {.code_bytes = bytes, .code_size = size};
    SDL_GPUShaderCreateInfo ci =
        shader_create_info_from_core_shader(&crs, SDL_GPU_SHADERFORMAT_SPIRV, SDL_GPU_SHADERSTAGE_VERTEX, 0, 0, 0, 0);
    SDL_GPUShader* shader = SDL_CreateGPUShader(render_context->device, &ci);

    titus_log_info("Shader successfully created");
    SDL_ReleaseGPUShader(render_context->device, shader);
}

CORE_EXPORT void titus_deinitialize(titus_application_context* ctx) {
    ecs_entity_t ctx_comp = ecs_lookup(ctx->ecs, "core:render_context");
    titus_log_info("rc comp: %llu", ctx_comp);

    ecs_query_t* rc_query = ecs_query(ctx->ecs, {.terms = {{.id = ctx_comp}}});
    ecs_iter_t it         = ecs_query_iter(ctx->ecs, rc_query);

    while(ecs_query_next(&it)) {
        core_render_context* rc = ecs_field(&it, core_render_context, 0);
        SDL_DestroyGPUDevice(rc->device);
    }
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
