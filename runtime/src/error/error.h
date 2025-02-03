#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef enum titus_result_type {
    TITUS_RESULT_ERROR = 0,
    TITUS_RESULT_OK    = 1,
} titus_result_type;

typedef struct titus_result {
    titus_result_type type;
    const char* msg;
} titus_result;

titus_result titus_result_ok();
titus_result titus_result_err(const char* msg);

#ifdef __cplusplus
}
#endif