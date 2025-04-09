#pragma once

#include "titus/export.h"

// void titus_log_function(void* ud, int cat, SDL_LogPriority pri, const char* msg);
TITUS_EXPORT void titus_log_verbose(const char* fmt, ...);
TITUS_EXPORT void titus_log_trace(const char* fmt, ...);
TITUS_EXPORT void titus_log_debug(const char* fmt, ...);
TITUS_EXPORT void titus_log_info(const char* fmt, ...);
TITUS_EXPORT void titus_log_warn(const char* fmt, ...);
TITUS_EXPORT void titus_log_error(const char* fmt, ...);
