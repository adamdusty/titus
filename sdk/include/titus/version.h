#pragma once

#include <stdint.h>
#include <titus/sds/sds.h>

typedef struct TitusVersion {
    uint64_t major;
    uint64_t minor;
    uint64_t patch;
    sds annotation;
} TitusVersion;

TITUS_EXPORT sds titus_version_to_string(const TitusVersion* ver);
TITUS_EXPORT int32_t titus_version_compare(const TitusVersion* lhs, const TitusVersion* rhs);
TITUS_EXPORT bool titus_parse_version(const char* version_str, TitusVersion* out);
