#pragma once

#include "titus/export.h"
#include <flecs.h>


TITUS_EXPORT char* titus_get_asset_path(ecs_world_t* ecs, char* namespace, char* name, char* /* path */);
