#include "config/config.h"

#include <titus/assert/assert.h>

TitusConfig titus_default_config() {
    return (TitusConfig){
        .log_level             = "warn",
        .module_root_directory = "modules",
    };
}

TitusConfig titus_load_config(const char* path) {
    TITUS_ASSERT(false && "Unimplemented");
}
