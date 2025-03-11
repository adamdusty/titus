#pragma once

#include <SDL3/SDL.h>

SDL_GPUGraphicsPipeline* phong_create_pipeline(SDL_GPUDevice* device,
                                               SDL_GPUShader* vertex,
                                               SDL_GPUShader* fragment,
                                               SDL_GPUTextureFormat target_format);