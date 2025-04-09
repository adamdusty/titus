#include "components.h"
#include "context.h"
#include "gpu.h"
#include "renderer/export.h"
#include "shaders.h"
#include <SDL3/SDL.h>
#include <cglm/cglm.h>
#include <cglm/clipspace/view_rh_zo.h>
#include <core/core.h>
#include <titus/sdk.h>

sds core_renderer_resource_directory = NULL;

static SDL_GPUGraphicsPipeline* default_pipeline = NULL;
static SDL_GPUTexture* depth_texture             = NULL;
static ecs_query_t* camera_query                 = NULL;
static ecs_query_t* mesh_query                   = NULL;

ECS_COMPONENT_DECLARE(core_render_context);
ECS_COMPONENT_DECLARE(CoreMeshRenderInfo);
ECS_SYSTEM_DECLARE(render_frame);

void on_set_core_mesh(ecs_iter_t* it);
core_render_context* create_render_context(SDL_Window* window, ecs_world_t* ecs);
void render_frame(ecs_iter_t* it);

void rendererImport(ecs_world_t* ecs) {

    ECS_MODULE(ecs, CoreRenderer);

    ECS_COMPONENT_DEFINE(ecs, core_render_context);
    ECS_COMPONENT_DEFINE(ecs, CoreMeshRenderInfo);

    ECS_SYSTEM_DEFINE(ecs, render_frame, EcsPostUpdate, core.renderer.core_render_context($));

    ecs_observer(ecs,
                 {
                     .query.terms = {{ecs_id(CoreMesh)}},
                     .events      = EcsOnSet,
                     .callback    = on_set_core_mesh,
                 });
}

CORE_RENDERER_EXPORT void titus_initialize(const TitusApplicationContext* ctx, sds root) {
    titus_log_info("Initializing core.renderer module");

    core_renderer_resource_directory = sdsdup(root);
    core_renderer_resource_directory = sdscat(core_renderer_resource_directory, "resources/");

    ecs_entity_t m = ecs_import(ctx->ecs, rendererImport, "core.renderer");
    if(0 == m) {
        titus_log_error("Failed to load module");
        return;
    }

    core_render_context* render_context = create_render_context(ctx->window, ctx->ecs);
    if(NULL == render_context) {
        titus_log_error("Failed to create render context");
    }

    SDL_SetGPUSwapchainParameters(
        render_context->device, render_context->window, SDL_GPU_SWAPCHAINCOMPOSITION_SDR, SDL_GPU_PRESENTMODE_VSYNC);

    default_pipeline = create_default_pipeline(ctx->ecs, render_context);

    int width, height;
    SDL_GetWindowSizeInPixels(render_context->window, &width, &height);
    SDL_GPUTextureCreateInfo depth_ci = {
        .type                 = SDL_GPU_TEXTURETYPE_2D,
        .width                = width,
        .height               = height,
        .layer_count_or_depth = 1,
        .num_levels           = 1,
        .format               = SDL_GPU_TEXTUREFORMAT_D16_UNORM,
        .usage                = SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET,
    };
    depth_texture = SDL_CreateGPUTexture(render_context->device, &depth_ci);

    ECS_QUERY(ctx->ecs, cameras, core.CoreCamera);

    camera_query =
        ecs_query(ctx->ecs, {.expr = "[in] core.CoreCamera, [in] core.CorePosition", .cache_kind = EcsQueryCacheAuto});
    mesh_query = ecs_query(
        ctx->ecs,
        {.expr = "[in] core.CoreMesh, [in] core.renderer.CoreMeshRenderInfo", .cache_kind = EcsQueryCacheAuto});
}

CORE_RENDERER_EXPORT void titus_deinitialize(TitusApplicationContext* ctx) {
    titus_log_info("Deinitializing core.renderer");
    const core_render_context* rend = ecs_singleton_get(ctx->ecs, core_render_context);

    ecs_iter_t mesh_info_iter = ecs_query_iter(ctx->ecs, mesh_query);
    while(ecs_iter_next(&mesh_info_iter)) {
        CoreMesh* _            = ecs_field(&mesh_info_iter, CoreMesh, 0);
        CoreMeshRenderInfo* ri = ecs_field(&mesh_info_iter, CoreMeshRenderInfo, 1);

        for(int i = 0; i < mesh_info_iter.count; i++) {
            SDL_ReleaseGPUBuffer(rend->device, ri[i].vertex_buffer);
            SDL_ReleaseGPUBuffer(rend->device, ri[i].index_buffer);
        }
    }

    SDL_ReleaseGPUTexture(rend->device, depth_texture);

    SDL_ReleaseGPUGraphicsPipeline(rend->device, default_pipeline);
    SDL_DestroyGPUDevice(rend->device);
}

void render_frame(ecs_iter_t* it) {
    core_render_context* ctx = ecs_field(it, core_render_context, 0);

    ecs_iter_t camera_it = ecs_query_iter(it->world, camera_query);
    ecs_iter_t mesh_it   = ecs_query_iter(it->world, mesh_query);

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

    if(swapchainTexture == NULL) {
        titus_log_info("Swapchain texture is NULL");
        return;
    }

    SDL_GPUColorTargetInfo colorTargetInfo = {0};
    colorTargetInfo.texture                = swapchainTexture;
    colorTargetInfo.clear_color            = from_rgba(100, 149, 237, 255); // cornflower blue
    colorTargetInfo.load_op                = SDL_GPU_LOADOP_CLEAR;
    colorTargetInfo.store_op               = SDL_GPU_STOREOP_DONT_CARE;

    SDL_GPUDepthStencilTargetInfo depth_target_info = {0};
    depth_target_info.texture                       = depth_texture;
    depth_target_info.cycle                         = true;
    depth_target_info.clear_depth                   = 1.0f;
    depth_target_info.clear_stencil                 = 0;
    depth_target_info.load_op                       = SDL_GPU_LOADOP_CLEAR;
    depth_target_info.store_op                      = SDL_GPU_STOREOP_STORE;
    depth_target_info.stencil_load_op               = SDL_GPU_LOADOP_CLEAR;
    depth_target_info.stencil_store_op              = SDL_GPU_STOREOP_STORE;

    // Render scene from every camera view
    while(ecs_query_next(&camera_it)) {
        CoreCamera* camera            = ecs_field(&camera_it, CoreCamera, 0);
        CorePosition* camera_position = ecs_field(&camera_it, CorePosition, 1);

        mat4 cam[2] = {0};

        glm_perspective(45.0f, 1280.0f / 720.0f, 0.01f, 1000.0f, cam[0]);

        vec3 center = {0};
        glm_vec3_add(*camera_position, camera->forward, center);
        glm_lookat(*camera_position, center, camera->up, cam[1]);
        SDL_PushGPUVertexUniformData(cmdbuf, 0, &cam, sizeof(mat4) * 2);

        SDL_GPURenderPass* render_pass = SDL_BeginGPURenderPass(cmdbuf, &colorTargetInfo, 1, &depth_target_info);
        if(NULL == render_pass) {
            titus_log_error("Failed to begin render pass: %s", SDL_GetError());
            return;
        }

        for(int i = 0; i < camera_it.count; i++) {
            // Render every mesh
            while(ecs_query_next(&mesh_it)) {
                CoreMesh* mesh                  = ecs_field(&mesh_it, CoreMesh, 0);
                CoreMeshRenderInfo* render_info = ecs_field(&mesh_it, CoreMeshRenderInfo, 1);

                for(int j = 0; j < mesh_it.count; j++) {
                    upload_vertex_data(ctx, render_info->vertex_buffer, &mesh[j]);
                    upload_index_data(ctx, render_info->index_buffer, &mesh[j]);

                    SDL_BindGPUGraphicsPipeline(render_pass, default_pipeline);
                    SDL_BindGPUVertexBuffers(
                        render_pass, 0, &(SDL_GPUBufferBinding){.buffer = render_info->vertex_buffer, .offset = 0}, 1);
                    SDL_BindGPUIndexBuffer(
                        render_pass, &(SDL_GPUBufferBinding){.buffer = render_info->index_buffer, .offset = 0}, 1);
                    SDL_DrawGPUIndexedPrimitives(render_pass, mesh[j].index_count, 1, 0, 0, 0);
                }
            }
        }

        SDL_EndGPURenderPass(render_pass);
    }

    SDL_SubmitGPUCommandBuffer(cmdbuf);

    // ecs_iter_fini(&mesh_it);
    // ecs_iter_fini(&camera_it);

    return;
}

core_render_context* create_render_context(SDL_Window* window, ecs_world_t* ecs) {
    core_render_context* render_context = ecs_singleton_ensure(ecs, core_render_context);

    SDL_GPUDevice* gpu_device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, "vulkan");
    if(NULL == gpu_device) {
        titus_log_error("Error initializing GPU device from SDL: %s", SDL_GetError());
        return NULL;
    }

    if(NULL == window) {
        titus_log_error("Application window is null");
        return NULL;
    }

    if(!SDL_ClaimWindowForGPUDevice(gpu_device, window)) {
        titus_log_error("Failed to claim GPU device for window: %s", SDL_GetError());
        return NULL;
    }

    render_context->window = window;
    render_context->device = gpu_device;

    return render_context;
}

void on_set_core_mesh(ecs_iter_t* it) {
    CoreMesh* mesh = ecs_field(it, CoreMesh, 0);

    titus_log_debug("CoreMesh component was set on some entity");

    const core_render_context* rc = ecs_singleton_get(it->world, core_render_context);

    for(int i = 0; i < it->count; i++) {
        CoreMeshRenderInfo* info = ecs_ensure(it->world, it->entities[i], CoreMeshRenderInfo);
        core_init_mesh_vertex_buffer(rc->device, mesh, &info->vertex_buffer);
        core_init_mesh_index_buffer(rc->device, mesh, &info->index_buffer);

        if(NULL == info->vertex_buffer || NULL == info->index_buffer) {
            titus_log_error("Failed to create GPU buffers for mesh");
        }
    }
}
