#include "titus/log.h"

#include <SDL3/SDL.h>
#include <stdarg.h>

void titus_log_verbose(const char* fmt, ...) {
    va_list args;

    va_start(args, fmt);
    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_VERBOSE, fmt, args);
    va_end(args);
}

void titus_log_trace(const char* fmt, ...) {
    va_list args;

    va_start(args, fmt);
    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_TRACE, fmt, args);
    va_end(args);
}

void titus_log_debug(const char* fmt, ...) {
    va_list args;

    va_start(args, fmt);
    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG, fmt, args);
    va_end(args);
}

void titus_log_info(const char* fmt, ...) {
    va_list args;

    va_start(args, fmt);
    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, fmt, args);
    va_end(args);
}

void titus_log_warn(const char* fmt, ...) {
    va_list args;

    va_start(args, fmt);
    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_WARN, fmt, args);
    va_end(args);
}

void titus_log_error(const char* fmt, ...) {
    va_list args;

    va_start(args, fmt);
    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, fmt, args);
    va_end(args);
}
