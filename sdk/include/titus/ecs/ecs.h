#pragma once

#include "titus/export.h"
#include <flecs.h>

#define TITUS_REGISTER_COMPONENT(W, N, T) titus_register_component(W, N, #T, sizeof(T), alignof(T))
#define TITUS_REGISTER_COMPONENT_NAMED(W, NS, CN, T) titus_register_component(W, NS, CN, sizeof(T), alignof(T))

TITUS_EXPORT ecs_entity_t titus_register_component(
    ecs_world_t* world, const char* nspace, const char* comp_name, ecs_size_t size, ecs_size_t alignment);
