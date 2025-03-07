#pragma once

#include "core/export.h"
#include <flecs.h>

extern int CORE_MODULE_EXPORT core_module_version;

extern CORE_MODULE_EXPORT ECS_COMPONENT_DECLARE(CoreFrameInput);
extern CORE_MODULE_EXPORT ECS_COMPONENT_DECLARE(CoreMesh);
extern CORE_MODULE_EXPORT ECS_COMPONENT_DECLARE(CoreCamera);

extern CORE_MODULE_EXPORT ECS_COMPONENT_DECLARE(CorePosition);
extern CORE_MODULE_EXPORT ECS_COMPONENT_DECLARE(CoreScale);
extern CORE_MODULE_EXPORT ECS_COMPONENT_DECLARE(CoreRotation);
