#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct titus_result {
    enum {
        TITUS_RESULT_OK    = 0,
        TITUS_RESULT_ERROR = 1,
    } type;
    const char* msg;
} titus_result;

titus_result titus_result_ok();
titus_result titus_result_err(const char* msg);

#ifdef __cplusplus
}
#endif
