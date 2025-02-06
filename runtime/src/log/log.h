#pragma once

#include <SDL3/SDL.h>

#define log_verbose(fmt, ...) SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, fmt, __VA_ARGS__)
#define log_trace(fmt, ...) SDL_LogTrace(SDL_LOG_CATEGORY_APPLICATION, fmt, __VA_ARGS__)
#define log_debug(fmt, ...) SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, fmt, __VA_ARGS__)
#define log_info(fmt, ...) SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, fmt, __VA_ARGS__)
#define log_warn(fmt, ...) SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, fmt, __VA_ARGS__)
#define log_error(fmt, ...) SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, fmt, __VA_ARGS__)

// void titus_log_function(void* ud, int cat, SDL_LogPriority pri, const char* msg) {
//     SDL_IOStream* s = (SDL_IOStream*)ud;
//     if(NULL != s) {
//         SDL_WriteIO(s, msg, SDL_strlen(msg));
//     }
// }