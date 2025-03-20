#pragma once

#include "titus/assert/assert.h"
#include "titus/context/context.h"
#include "titus/ds/stb_ds.h"
#include "titus/log/log.h"
#include "titus/sds/sds.h"
#include <SDL3/SDL.h>
#include <flecs.h>

typedef struct TitusVersion {
    uint64_t major;
    uint64_t minor;
    uint64_t patch;
    sds annotation;
} TitusVersion;

TITUS_EXPORT sds titus_version_to_string(const TitusVersion* ver);
TITUS_EXPORT int32_t titus_version_compare(const TitusVersion* lhs, const TitusVersion* rhs);
TITUS_EXPORT bool titus_parse_version(char* version_str, TitusVersion* out);
