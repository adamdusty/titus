#pragma once

#include "titus/sds/sds.h"
#include <SDL3/SDL.h>
#include <stdint.h>
#include <titus/module/module.h>
#include <titus/sdk.h>

#define TITUS_MODULE_INITIALIZE "titus_initialize"
#define TITUS_MODULE_DEINITIALIZE "titus_deinitialize"

extern ECS_COMPONENT_DECLARE(TitusModulePathInfo);
extern ECS_COMPONENT_DECLARE(TitusModuleMetaData);
extern ECS_COMPONENT_DECLARE(TitusModule);

typedef void (*titus_initialize_proc)(const TitusApplicationContext*);
typedef void (*titus_deinitialize_proc)(const TitusApplicationContext*);

typedef struct TitusModulePathInfo {
    sds root_directory;
    sds resource_directory;
    sds binary_path;
    TitusVersion version;
} TitusModulePathInfo;

typedef struct TitusModuleMetaData {
    sds namespace;
    sds name;
    sds binary_file_name;
    TitusVersion version;
} TitusModuleMetaData;

typedef struct TitusModule {
    TitusModuleMetaData metadata;
    SDL_SharedObject* binary;

    titus_initialize_proc initialize;
    titus_initialize_proc deinitialize;
} TitusModule;

// Get available modules and return a stb dynamic array
void titus_get_available_modules(ecs_world_t* ecs, const char* root);

// Load modules from a stb dynamic array of module metadata
void titus_load_modules(ecs_world_t* ecs, TitusModuleMetaData* module_metadatas);

// Unloads module and frees metadata
void titus_free_module(TitusModule* module);

// Deserialize a module manifest file to get it's metadata
bool titus_deserialize_manifest(sds path, TitusModuleMetaData* out);
