#pragma once

#include "module/module.h"
#include <titus/sds/sds.h>

typedef struct TitusModulePack {
    sds name;
    size_t module_count;
    TitusModuleMetaData* modules;
} TitusModulePack;

// Get all available packs from `root`
// Unimplemented
TitusModulePack* titus_get_module_packs(sds root);
