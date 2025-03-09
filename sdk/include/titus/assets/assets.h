#pragma once

#include "titus/export.h"
#include "titus/sds/sds.h"
#include <flecs.h>

TITUS_EXPORT sds titus_get_asset_path(ecs_world_t* ecs, char* namespace, char* name, char* path);
