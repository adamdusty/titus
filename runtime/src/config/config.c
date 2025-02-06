#include "config/config.h"

#include "assert/assert.h"

titus_config titus_default_config() {
    return (titus_config){
        .log_level             = "warn",
        .module_root_directory = "modules",
    };
}

titus_config titus_load_config(const char* path) {
    TITUS_ASSERT(false && "Unimplemented");
}
