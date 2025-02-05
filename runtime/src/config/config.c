#include "config/config.h"

titus_config titus_default_config() {
    return (titus_config){.module_root_directory = "modules"};
}

titus_config titus_load_config(const char* path) {
    titus_config c = titus_default_config();
    return c;
}
