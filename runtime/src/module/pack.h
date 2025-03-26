#pragma once

#include "module/module.h"
#include <stddef.h>
#include <titus/sds/sds.h>
#include <yyjson.h>

typedef struct TitusRequiredModule {
    sds nspace;
    sds name;
    TitusVersion version;
} TitusRequiredModule;

typedef struct TitusModulePack {
    sds name;                     // Owner: name of pack
    TitusRequiredModule* modules; // Owner: array of module meta data
    size_t module_count;
} TitusModulePack;

TitusModulePack titus_module_pack_create(sds name, TitusRequiredModule* modules, size_t count);
void titus_module_pack_destroy(TitusModulePack* pack);
TitusModulePack* titus_get_module_packs(sds root);
