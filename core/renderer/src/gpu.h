#pragma once

#include <context.h>
#include <core/core.h>

uint8_t* load_spirv(const char* path, size_t* size);

SDL_FColor from_rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void upload_vertex_data(core_render_context* ctx, SDL_GPUBuffer* buffer, const CoreMesh* mesh);
void upload_index_data(core_render_context* ctx, SDL_GPUBuffer* buffer, const CoreMesh* mesh);

void core_init_mesh_vertex_buffer(SDL_GPUDevice* dev, const CoreMesh* mesh, SDL_GPUBuffer** vertex_buffer);
void core_init_mesh_index_buffer(SDL_GPUDevice* dev, const CoreMesh* mesh, SDL_GPUBuffer** index_buffer);