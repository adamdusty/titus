#include "config/config.h"

#include <titus/assert/assert.h>

TitusConfig titus_default_config() {
    return (TitusConfig){
        .log_level        = "warn",
        .module_directory = "modules",
        .pack_directory   = "packs",
    };
}

TitusConfig titus_load_config(const char* path) {
    TITUS_ASSERT(false && "Unimplemented");
}
