#pragma once

#include <stdint.h>
/*
    Rendering components largely mimic SDL3_GPU, because the core renderer uses it, but I want to make it possible to
   implement a renderer with another API based on the same components.
*/

typedef enum CORE_RENDER_SHADER_STAGE {
    CORE_RENDER_SHADER_STAGE_VERTEX,
    CORE_RENDER_SHADER_STAGE_FRAGMENT,
} CORE_RENDER_SHADER_STAGE;

typedef enum CORE_RENDER_SHADER_FORMAT {
    CORE_RENDER_SHADER_FORMAT_INVALID  = 0,
    CORE_RENDER_SHADER_FORMAT_PRIVATE  = (1u << 0), /**< Shaders for NDA'd platforms. */
    CORE_RENDER_SHADER_FORMAT_SPIRV    = (1u << 1), /**< SPIR-V shaders for Vulkan. */
    CORE_RENDER_SHADER_FORMAT_DXBC     = (1u << 2), /**< DXBC SM5_1 shaders for D3D12. */
    CORE_RENDER_SHADER_FORMAT_DXIL     = (1u << 3), /**< DXIL SM6_0 shaders for D3D12. */
    CORE_RENDER_SHADER_FORMAT_MSL      = (1u << 4), /**< MSL shaders for Metal. */
    CORE_RENDER_SHADER_FORMAT_METALLIB = (1u << 5), /**< Precompiled metallib shaders for Metal. */
} CORE_RENDER_SHADER_FORMAT;

typedef struct core_render_shader {
    size_t code_size;
    uint8_t* code_bytes;
} core_render_shader;

typedef struct core_render_material {
} core_render_material;