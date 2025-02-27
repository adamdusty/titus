#include "context.h"
#include "core/export.h"
#include "shaders.h"
#include <SDL3/SDL.h>
#include <core/components.h>
#include <core/core.h>
#include <stdlib.h>
#include <titus/sdk.h>

static SDL_GPUGraphicsPipeline* default_pipeline = NULL;
static SDL_GPUBuffer* vertex_buffer              = NULL;
static ecs_query_t* camera_query                 = NULL;
static ecs_query_t* mesh_query                   = NULL;

void upload_vertex_data(core_render_context* ctx, SDL_GPUBuffer* buffer, const Core_Mesh* mesh);
void render_frame(ecs_iter_t* it);

SDL_FColor from_rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    return (SDL_FColor){.r = r / 255.0f, .g = g / 255.0f, .b = b / 255.0f, .a = a / 255.0f};
}

ECS_COMPONENT_DECLARE(core_render_context);
ECS_SYSTEM_DECLARE(render_frame);

void rendererImport(ecs_world_t* ecs) {

    ECS_MODULE(ecs, CoreRenderer);

    ECS_COMPONENT_DEFINE(ecs, core_render_context);

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

    size_t capsule_size = 0;
    vec3f* capsule      = core_capsule_mesh_positions(0.1f, 0.4f, 10, 10, &capsule_size);

    default_pipeline = create_default_pipeline(ctx->ecs, render_context);
    vertex_buffer    = SDL_CreateGPUBuffer(render_context->device,
                                        &(SDL_GPUBufferCreateInfo){
                                               .usage = SDL_GPU_BUFFERUSAGE_VERTEX,
                                               .size  = sizeof(Core_VertexPosition) * capsule_size,
                                        });

    // Core_Mesh test_mesh    = {0};
    // test_mesh.vertices     = verts;
    // test_mesh.vertex_count = 4;

    camera_query = ecs_query(ctx->ecs, {.expr = "[in] core.Core_Camera", .cache_kind = EcsQueryCacheAuto});
    mesh_query   = ecs_query(ctx->ecs, {.expr = "[in] core.Core_Mesh", .cache_kind = EcsQueryCacheAuto});

    ecs_entity_t camera_component = ecs_lookup(ctx->ecs, "core.Core_Camera");
    ecs_entity_t cam              = ecs_entity(ctx->ecs, {.name = "main_camera"});
    ecs_set_id(ctx->ecs,
               cam,
               camera_component,
               sizeof(Core_Camera),
               &(Core_Camera){
                   .up       = {0, 1.0f, 0},
                   .forward  = {0, 0, 1.0f},
                   .position = {0, 0, 0},
               });

    ecs_entity_t mesh_component = ecs_lookup(ctx->ecs, "core.Core_Mesh");
    ecs_entity_t test_e         = ecs_entity(ctx->ecs, {.name = "test_mesh"});
    Core_Mesh* test_mesh        = ecs_ensure_id(ctx->ecs, test_e, mesh_component);
    test_mesh->vertices         = malloc(sizeof(Core_VertexPosition) * capsule_size);
    SDL_memset(test_mesh->vertices, 0, sizeof(Core_VertexPosition) * capsule_size);

    for(size_t i = 0; i < capsule_size; ++i) {
        test_mesh->vertices[i] = (Core_VertexPosition){capsule[i].x, capsule[i].y, capsule[i].z};
    }

    test_mesh->vertex_count = capsule_size;
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

    // Render scene from every camera view
    while(ecs_query_next(&camera_it)) {
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

        Core_Camera* camera = ecs_field(&camera_it, Core_Camera, 0);

        SDL_GPUColorTargetInfo colorTargetInfo = {0};
        colorTargetInfo.texture                = swapchainTexture;
        colorTargetInfo.clear_color            = from_rgba(100, 149, 237, 255); // cornflower blue
        colorTargetInfo.load_op                = SDL_GPU_LOADOP_CLEAR;
        colorTargetInfo.store_op               = SDL_GPU_STOREOP_DONT_CARE;

        for(int i = 0; i < camera_it.count; i++) {

            SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(cmdbuf, &colorTargetInfo, 1, NULL);

            // Render every mesh
            while(ecs_query_next(&mesh_it)) {
                Core_Mesh* mesh = ecs_field(&mesh_it, Core_Mesh, 0);

                for(int j = 0; j < mesh_it.count; j++) {
                    upload_vertex_data(ctx, vertex_buffer, &mesh[j]);

                    SDL_BindGPUGraphicsPipeline(renderPass, default_pipeline);
                    SDL_BindGPUVertexBuffers(
                        renderPass, 0, &(SDL_GPUBufferBinding){.buffer = vertex_buffer, .offset = 0}, 1);
                    SDL_DrawGPUPrimitives(renderPass, mesh[j].vertex_count, 1, 0, 0);
                }
            }

            SDL_EndGPURenderPass(renderPass);
        }

        SDL_SubmitGPUCommandBuffer(cmdbuf);
    }

    return;
}

void upload_vertex_data(core_render_context* ctx, SDL_GPUBuffer* buffer, const Core_Mesh* mesh) {
    SDL_GPUTransferBuffer* transfer =
        SDL_CreateGPUTransferBuffer(ctx->device,
                                    &(SDL_GPUTransferBufferCreateInfo){
                                        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
                                        .size  = sizeof(mesh->vertices[0]) * mesh->vertex_count,
                                    });

    Core_VertexPosition* transfer_data = SDL_MapGPUTransferBuffer(ctx->device, transfer, false);

    for(size_t i = 0; i < mesh->vertex_count; i++) {
        transfer_data[i] = mesh->vertices[i];
    }

    SDL_UnmapGPUTransferBuffer(ctx->device, transfer);

    SDL_GPUCommandBuffer* upload = SDL_AcquireGPUCommandBuffer(ctx->device);
    SDL_GPUCopyPass* copy        = SDL_BeginGPUCopyPass(upload);
    SDL_UploadToGPUBuffer(copy,
                          &(SDL_GPUTransferBufferLocation){.transfer_buffer = transfer, .offset = 0},
                          &(SDL_GPUBufferRegion){
                              .buffer = buffer,
                              .offset = 0,
                              .size   = sizeof(mesh->vertices[0]) * mesh->vertex_count,
                          },
                          false);
    SDL_EndGPUCopyPass(copy);
    SDL_SubmitGPUCommandBuffer(upload);
    SDL_ReleaseGPUTransferBuffer(ctx->device, transfer);
}
