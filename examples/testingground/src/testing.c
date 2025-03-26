#include <titus/sdk.h>

#include "testing/export.h"

TG_EXPORT void titus_initialize(const TitusApplicationContext*, sds module_root_directory) {
    titus_log_info("Hello from testing");
}