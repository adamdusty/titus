#include "gpu.h"

#include <titus/log/log.h>

SDL_FColor from_rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    return (SDL_FColor){.r = r / 255.0f, .g = g / 255.0f, .b = b / 255.0f, .a = a / 255.0f};
}

void upload_vertex_data(core_render_context* ctx, SDL_GPUBuffer* buffer, const CoreMesh* mesh) {
    SDL_GPUTransferBuffer* transfer =
        SDL_CreateGPUTransferBuffer(ctx->device,
                                    &(SDL_GPUTransferBufferCreateInfo){
                                        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
                                        .size  = sizeof(mesh->vertices[0]) * mesh->vertex_count,
                                    });

    CoreVertexPositionNormal* transfer_data = SDL_MapGPUTransferBuffer(ctx->device, transfer, false);

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

void upload_index_data(core_render_context* ctx, SDL_GPUBuffer* buffer, const CoreMesh* mesh) {
    SDL_GPUTransferBuffer* transfer =
        SDL_CreateGPUTransferBuffer(ctx->device,
                                    &(SDL_GPUTransferBufferCreateInfo){
                                        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
                                        .size  = sizeof(mesh->indices[0]) * mesh->index_count,
                                    });

    uint32_t* transfer_data = SDL_MapGPUTransferBuffer(ctx->device, transfer, false);

    for(size_t i = 0; i < mesh->index_count; i++) {
        transfer_data[i] = mesh->indices[i];
    }

    SDL_UnmapGPUTransferBuffer(ctx->device, transfer);

    SDL_GPUCommandBuffer* upload = SDL_AcquireGPUCommandBuffer(ctx->device);
    SDL_GPUCopyPass* copy        = SDL_BeginGPUCopyPass(upload);
    SDL_UploadToGPUBuffer(copy,
                          &(SDL_GPUTransferBufferLocation){.transfer_buffer = transfer, .offset = 0},
                          &(SDL_GPUBufferRegion){
                              .buffer = buffer,
                              .offset = 0,
                              .size   = sizeof(mesh->indices[0]) * mesh->index_count,
                          },
                          false);
    SDL_EndGPUCopyPass(copy);
    SDL_SubmitGPUCommandBuffer(upload);
    SDL_ReleaseGPUTransferBuffer(ctx->device, transfer);
}

void core_init_mesh_vertex_buffer(SDL_GPUDevice* dev, const CoreMesh* mesh, SDL_GPUBuffer** vertex_buffer) {
    *vertex_buffer = SDL_CreateGPUBuffer(dev,
                                         &(SDL_GPUBufferCreateInfo){
                                             .usage = SDL_GPU_BUFFERUSAGE_VERTEX,
                                             .size  = sizeof(mesh->vertices[0]) * mesh->vertex_count,
                                         });
    if(vertex_buffer == NULL)
        titus_log_error("Failed to create vertex buffer");
}

void core_init_mesh_index_buffer(SDL_GPUDevice* dev, const CoreMesh* mesh, SDL_GPUBuffer** index_buffer) {
    *index_buffer = SDL_CreateGPUBuffer(dev,
                                        &(SDL_GPUBufferCreateInfo){
                                            .usage = SDL_GPU_BUFFERUSAGE_INDEX,
                                            .size  = sizeof(mesh->indices[0]) * mesh->index_count,
                                        });
}
