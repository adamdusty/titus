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

typedef struct TitusRequiredModuleDeserializationResult {
    TitusRequiredModule module;
    const char* error;
} TitusRequiredModuleDeserializationResult;

TitusModulePackDeserializationResult titus_module_pack_deserialize(yyjson_val* json);
TitusModuleMetaDataDeserializationResult titus_module_meta_deserialize(yyjson_val* json);
TitusRequiredModuleDeserializationResult titus_required_module_deserialize(yyjson_val* json);
