#include "components.h"
#include "context.h"
#include "core/export.h"
#include "gpu.h"
#include "shaders.h"
#include <SDL3/SDL.h>
#include <cglm/cglm.h>
#include <core/components.h>
#include <core/core.h>
#include <stdlib.h>
#include <titus/sdk.h>

/* TODO
    - Add uniforms to matrix
    - Add uniforms to pipeline
*/

static SDL_GPUGraphicsPipeline* default_pipeline = NULL;
static SDL_GPUBuffer* camera_buffer              = NULL;
static ecs_query_t* camera_query                 = NULL;
static ecs_query_t* mesh_query                   = NULL;

void render_frame(ecs_iter_t* it);

ECS_COMPONENT_DECLARE(core_render_context);
ECS_COMPONENT_DECLARE(CoreMeshRenderInfo);
ECS_SYSTEM_DECLARE(render_frame);

void rendererImport(ecs_world_t* ecs) {

    ECS_MODULE(ecs, CoreRenderer);

    ECS_COMPONENT_DEFINE(ecs, core_render_context);
    ECS_COMPONENT_DEFINE(ecs, CoreMeshRenderInfo);

    ECS_SYSTEM_DEFINE(ecs, render_frame, EcsPostUpdate, core.renderer.core_render_context($));
}

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

    // size_t capsule_size = 0;
    CoreMesh capsule = core_create_capsule_mesh(0.1f, 0.4f, 10, 10);

    ecs_entity_t test_e = ecs_entity(ctx->ecs, {.name = "test_mesh"});
    ecs_set(ctx->ecs,
            test_e,
            CoreMesh,
            {
                .vertices     = capsule.vertices,
                .vertex_count = capsule.vertex_count,
                .indices      = capsule.indices,
                .index_count  = capsule.index_count,
            });

    default_pipeline = create_default_pipeline(ctx->ecs, render_context);

    camera_query =
        ecs_query(ctx->ecs, {.expr = "[in] core.CoreCamera, [in] core.CorePosition", .cache_kind = EcsQueryCacheAuto});
    mesh_query = ecs_query(
        ctx->ecs,
        {.expr = "[in] core.CoreMesh, [in] core.renderer.CoreMeshRenderInfo", .cache_kind = EcsQueryCacheAuto});

    ecs_entity_t cam = ecs_entity(ctx->ecs, {.name = "main_camera"});
    ecs_set(ctx->ecs, cam, CorePosition, {.x = 0.0f, .y = 0.0f, .z = -10.0f});
    ecs_set(ctx->ecs,
            cam,
            CoreCamera,
            {
                .up      = {0, 1.0f, 0},
                .forward = {0, 0, 1.0f},
            });

    ecs_query_t* mesh_no_info_q  = ecs_query(ctx->ecs, {.expr = "core.CoreMesh, !core.renderer.CoreMeshRenderInfo"});
    ecs_iter_t mesh_no_info_iter = ecs_query_iter(ctx->ecs, mesh_no_info_q);
    while(ecs_query_next(&mesh_no_info_iter)) {
        CoreMesh* mesh = ecs_field(&mesh_no_info_iter, CoreMesh, 0);

        for(int i = 0; i < mesh_no_info_iter.count; i++) {
            CoreMeshRenderInfo* info = ecs_ensure(ctx->ecs, mesh_no_info_iter.entities[i], CoreMeshRenderInfo);
            core_init_mesh_vertex_buffer(render_context->device, mesh, &info->vertex_buffer);
            core_init_mesh_index_buffer(render_context->device, mesh, &info->index_buffer);

            if(NULL == info->vertex_buffer || NULL == info->index_buffer) {
                titus_log_error("Failed to create GPU buffers for mesh");
            }
        }
    }
}

CORE_EXPORT void titus_deinitialize(titus_application_context* ctx) {
    titus_log_info("Deinitializing core.renderer");

    const core_render_context* rend = ecs_singleton_get(ctx->ecs, core_render_context);
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

    // Render scene from every camera view
    while(ecs_query_next(&camera_it)) {
        CoreCamera* camera            = ecs_field(&camera_it, CoreCamera, 0);
        CorePosition* camera_position = ecs_field(&camera_it, CorePosition, 1);

        float pos[3] = {camera_position->x, camera_position->y, camera_position->z};
        float up[3]  = {camera->up.x, camera->up.y, camera->up.z};
        mat4 cam[2]  = {0};

        glm_perspective(45.0f, 1280.0f / 720.0f, 0.01f, 100.0f, cam[0]);
        glm_lookat(pos, (float[]){0.0f, 0.0f, 0.0f}, up, cam[1]);
        SDL_PushGPUVertexUniformData(cmdbuf, 0, &cam, sizeof(mat4) * 2);

        SDL_GPURenderPass* render_pass = SDL_BeginGPURenderPass(cmdbuf, &colorTargetInfo, 1, NULL);
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
        SDL_SubmitGPUCommandBuffer(cmdbuf);
    }

    return;
}