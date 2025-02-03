#include "config/config.h"

titus_config titus_load_config(const char* path) {
    titus_config c = {.test = "hello config"};
    return c;
}
