#pragma once

#include "module/module.h"
#include <titus/sds/sds.h>

typedef struct TitusModulePack {
    sds name;
    size_t module_count;
    TitusModuleMetaData* required_modules;
} TitusModulePack;

TitusModulePack* titus_get_module_packs(const char* root);
