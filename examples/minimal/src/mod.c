#include <SDL3/SDL.h>
#include <flecs.h>
#include <stdio.h>
#include <titus/sdk.h>

#if defined(_WIN32)
#define PLUGIN_EXPORT __declspec(dllexport)
#elif defined(__linux__)
#define PLUGIN_EXPORT __attribute__((visibility("default")))
#endif

PLUGIN_EXPORT void titus_initialize(TitusApplicationContext* ctx) {
    printf("Hello from minimal plugin initialization\n");
    ecs_entity_t in = ecs_lookup(ctx->ecs, "core:input");
    if(in == 0) {
        printf("did not find input component\n");
    } else {
        printf("found input component\n");
    }

    ecs_entity_t wc = ecs_lookup(ctx->ecs, "runtime:window");
    SDL_Window* w   = *(SDL_Window**)ecs_get_mut_id(ctx->ecs, wc, wc);
    if(w == NULL) {
        printf("Failed to get window\n");
    }

    int width, height;
    if(SDL_GetWindowSize(w, &width, &height)) {
        printf("Got window from ecs world: [%d, %d]\n", width, height);
    } else {
        printf("Error getting window size: %s", SDL_GetError());
    }
}

PLUGIN_EXPORT void titus_deinitialize(void* ud) {
    printf("Hello from minimal plugin deinitialization\n");
}