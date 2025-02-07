#include "titus/ecs/ecs.h"

#include <SDL3/SDL.h>

ecs_entity_t titus_register_component(
    ecs_world_t* world, const char* nspace, const char* comp_name, ecs_size_t size, ecs_size_t alignment) {
    size_t nspace_size    = SDL_strlen(nspace);
    size_t comp_name_size = SDL_strlen(comp_name);
    size_t name_size      = nspace_size + comp_name_size + 2;
    char name[name_size]  = {};
    memset(name, 0, name_size);
    SDL_snprintf(name, name_size, "%s:%s", nspace, comp_name);

    ecs_component_desc_t desc = {0};
    ecs_entity_desc_t edesc   = {0};
    edesc.use_low_id          = true;
    edesc.name                = name;
    desc.entity               = ecs_entity_init(world, &edesc);
    desc.type.size            = size;
    desc.type.alignment       = alignment;
    return ecs_component_init(world, &desc);
}
