#pragma once

typedef struct TitusConfig {
    const char* log_level;
    const char* module_root_directory;
} TitusConfig;

TitusConfig titus_load_config(const char* path);
TitusConfig titus_default_config();
