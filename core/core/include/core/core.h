#pragma once

#include "core/export.h"
#include "core/input.h"
#include "core/rendering.h"

#include <cglm/cglm.h>
#include <flecs.h>

typedef vec3 CorePosition;
typedef vec3 CoreScale;
typedef vec3 CoreRotation;

typedef mat4 CoreTransform;

extern CORE_MODULE_EXPORT ECS_COMPONENT_DECLARE(CoreFrameInput);
extern CORE_MODULE_EXPORT ECS_COMPONENT_DECLARE(CoreFrameInputComponent);

extern CORE_MODULE_EXPORT ECS_COMPONENT_DECLARE(CoreMesh);
extern CORE_MODULE_EXPORT ECS_COMPONENT_DECLARE(CoreCamera);

extern CORE_MODULE_EXPORT ECS_COMPONENT_DECLARE(CorePosition);
extern CORE_MODULE_EXPORT ECS_COMPONENT_DECLARE(CoreScale);
extern CORE_MODULE_EXPORT ECS_COMPONENT_DECLARE(CoreRotation);
