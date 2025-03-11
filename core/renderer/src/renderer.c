#include "components.h"
#include "context.h"
#include "gpu.h"
#include "renderer/export.h"
#include "shaders.h"
#include <SDL3/SDL.h>
#include <cglm/cglm.h>
#include <cglm/clipspace/view_rh_zo.h>
#include <core/core.h>
#include <stdlib.h>
#include <titus/sdk.h>

CoreMesh create_cube_mesh();

static SDL_GPUGraphicsPipeline* default_pipeline = NULL;
static SDL_GPUTexture* depth_texture             = NULL;
static ecs_query_t* camera_query                 = NULL;
static ecs_query_t* mesh_query                   = NULL;

core_render_context* create_render_context(SDL_Window* window, ecs_world_t* ecs);
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

CORE_RENDERER_EXPORT void titus_initialize(const TitusApplicationContext* ctx) {
    titus_log_info("Initializing core.renderer module");

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

    // size_t capsule_size = 0;
    CoreMesh capsule = create_cube_mesh(); // core_create_capsule_mesh(1.0f, 2.0f, 10, 10);

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

    ecs_entity_t cam = ecs_entity(ctx->ecs, {.name = "main_camera"});
    ecs_set(ctx->ecs, cam, CorePosition, {0, 0, -100.0f});
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

CORE_RENDERER_EXPORT void titus_deinitialize(TitusApplicationContext* ctx) {
    titus_log_info("Deinitializing core.renderer");
    const core_render_context* rend = ecs_singleton_get(ctx->ecs, core_render_context);

    ecs_iter_t mesh_info_iter = ecs_query_iter(ctx->ecs, mesh_query);
    while(ecs_iter_next(&mesh_info_iter)) {
        CoreMesh* _            = ecs_field(&mesh_info_iter, CoreMesh, 0);
        CoreMeshRenderInfo* ri = ecs_field(&mesh_info_iter, CoreMeshRenderInfo, 1);

        for(int i = 0; i < mesh_info_iter.count; i++) {
            SDL_ReleaseGPUBuffer(rend->device, ri->vertex_buffer);
            SDL_ReleaseGPUBuffer(rend->device, ri->index_buffer);
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
        glm_lookat_rh_zo(*camera_position, (float[]){0.0f, 0.0f, 0.0f}, camera->up, cam[1]);
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
        SDL_SubmitGPUCommandBuffer(cmdbuf);
    }

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

CoreMesh create_cube_mesh() {
    CoreMesh mesh;

    // Define the 8 unique vertex positions of the cube
    vec3 positions[] = {
        {-0.5f, -0.5f, -0.5f}, // 0
        {0.5f, -0.5f, -0.5f},  // 1
        {0.5f, 0.5f, -0.5f},   // 2
        {-0.5f, 0.5f, -0.5f},  // 3
        {-0.5f, -0.5f, 0.5f},  // 4
        {0.5f, -0.5f, 0.5f},   // 5
        {0.5f, 0.5f, 0.5f},    // 6
        {-0.5f, 0.5f, 0.5f}    // 7
    };

    // Define the 6 face normals of the cube
    vec3 normals[] = {
        {0.0f, 0.0f, -1.0f}, // Back face
        {0.0f, 0.0f, 1.0f},  // Front face
        {-1.0f, 0.0f, 0.0f}, // Left face
        {1.0f, 0.0f, 0.0f},  // Right face
        {0.0f, -1.0f, 0.0f}, // Bottom face
        {0.0f, 1.0f, 0.0f}   // Top face
    };

    // Define the 12 triangles (2 per face) using the 8 unique vertices
    uint32_t face_indices[] = {
        // Back face
        0,
        1,
        2,
        2,
        3,
        0,

        // Front face
        4,
        5,
        6,
        6,
        7,
        4,

        // Left face
        0,
        3,
        7,
        7,
        4,
        0,

        // Right face
        1,
        5,
        6,
        6,
        2,
        1,

        // Bottom face
        0,
        1,
        5,
        5,
        4,
        0,

        // Top face
        3,
        2,
        6,
        6,
        7,
        3,
    };

    // For flat shading, each face has its own set of vertices with the same normal
    mesh.vertex_count = 36; // 6 faces * 6 vertices per face (2 triangles)
    mesh.vertices     = (CoreVertexPositionNormal*)malloc(mesh.vertex_count * sizeof(CoreVertexPositionNormal));

    mesh.index_count = 36; // 12 triangles * 3 indices per triangle
    mesh.indices     = (uint32_t*)malloc(mesh.index_count * sizeof(uint32_t));

    // Create vertices and assign normals
    for(size_t i = 0; i < mesh.index_count; ++i) {
        uint32_t vertex_index = face_indices[i];
        memcpy(mesh.vertices[i].position, positions[vertex_index], sizeof(vec3));

        // Assign the normal based on the face
        if(i < 6) {
            memcpy(mesh.vertices[i].normal, normals[0], sizeof(vec3)); // Back face
        } else if(i < 12) {
            memcpy(mesh.vertices[i].normal, normals[1], sizeof(vec3)); // Front face
        } else if(i < 18) {
            memcpy(mesh.vertices[i].normal, normals[2], sizeof(vec3)); // Left face
        } else if(i < 24) {
            memcpy(mesh.vertices[i].normal, normals[3], sizeof(vec3)); // Right face
        } else if(i < 30) {
            memcpy(mesh.vertices[i].normal, normals[4], sizeof(vec3)); // Bottom face
        } else {
            memcpy(mesh.vertices[i].normal, normals[5], sizeof(vec3)); // Top face
        }

        // Assign indices (just a linear sequence for flat shading)
        mesh.indices[i] = i;
    }

    return mesh;
}

/*

    // clang-format off
    mesh.vertices = (CoreVertexPositionNormal[]){
        {-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f},
         {0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f},
         {0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f},
         {0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f},
        {-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f},
        {-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f},

        {-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f},
         {0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f},
         {0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f},
         {0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f},
        {-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f},
        {-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f},

        {-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f},
        {-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f},
        {-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f},
        {-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f},
        {-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f},
        {-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f},

         {0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f},
         {0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f},
         {0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f},
         {0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f},
         {0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f},
         {0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f},

        {-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f},
         {0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f},
         {0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f},
         {0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f},
        {-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f},
        {-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f},

        {-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f},
         {0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f},
         {0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f},
         {0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f},
        {-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f},
        {-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f},
    };
    // clang-format on
*/