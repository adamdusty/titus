#include "error/error.h"

#include <stddef.h>

titus_result titus_result_ok() {
    return (titus_result){.type = TITUS_RESULT_OK, .msg = NULL};
}

titus_result titus_result_err(const char* msg) {
    return (titus_result){.type = TITUS_RESULT_ERROR, .msg = msg};
}
