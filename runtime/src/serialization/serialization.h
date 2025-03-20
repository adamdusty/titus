#pragma once

#include "module/module.h"
#include "module/pack.h"

typedef struct TitusModulePackDeserializationResult {
    TitusModulePack pack;
    const char* error;
} TitusModulePackDeserializationResult;

typedef struct TitusModuleMetaDataDeserializationResult {
    TitusModuleMetaData meta;
    const char* error;
} TitusModuleMetaDataDeserializationResult;

TitusModulePackDeserializationResult titus_module_pack_deserialize(yyjson_val* json);
TitusModuleMetaDataDeserializationResult titus_module_meta_deserialize(yyjson_val* json);
