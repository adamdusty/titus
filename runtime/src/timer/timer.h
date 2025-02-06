#pragma once

#include <stdint.h>

typedef struct titus_timer {
    uint64_t ticks;
    float delta;
} titus_timer;
