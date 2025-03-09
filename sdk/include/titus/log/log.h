#pragma once

#include "titus/export.h"

// #define titus_log_verbose(fmt, ...) SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, fmt, ##__VA_ARGS__)
// #define titus_log_trace(fmt, ...) SDL_LogTrace(SDL_LOG_CATEGORY_APPLICATION, fmt, ##__VA_ARGS__)
// #define titus_log_debug(fmt, ...) SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, fmt, ##__VA_ARGS__)
// #define titus_log_info(fmt, ...) SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, fmt, ##__VA_ARGS__)
// #define titus_log_warn(fmt, ...) SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, fmt, ##__VA_ARGS__)
// #define titus_log_error(fmt, ...) SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, fmt, ##__VA_ARGS__)

// void titus_log_function(void* ud, int cat, SDL_LogPriority pri, const char* msg);
TITUS_EXPORT void titus_log_verbose(const char* fmt, ...);
TITUS_EXPORT void titus_log_trace(const char* fmt, ...);
TITUS_EXPORT void titus_log_debug(const char* fmt, ...);
TITUS_EXPORT void titus_log_info(const char* fmt, ...);
TITUS_EXPORT void titus_log_warn(const char* fmt, ...);
TITUS_EXPORT void titus_log_error(const char* fmt, ...);
