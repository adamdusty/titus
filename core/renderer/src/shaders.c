#include "shaders.h"

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

uint8_t* load_spirv(const char* path, size_t* size) {
    uint8_t* bytes = SDL_LoadFile(path, size);
    return bytes;
}