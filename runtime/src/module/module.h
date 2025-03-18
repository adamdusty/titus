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

typedef struct TitusModuleMetaData {
    sds namespace;
    sds name;
    TitusVersion version;
} TitusModuleMetaData;

typedef struct TitusModule {
    sds root_directory;
    TitusModuleMetaData* metadata;
    SDL_SharedObject* binary;

    titus_initialize_proc initialize;
    titus_initialize_proc deinitialize;
} TitusModule;

// Get all the modules available to be loaded. This just means they are in a subdirectory of `root` and have a
// module.json manifest file describing the module. Returns an stb dynamic array of load info.
TitusModule* titus_get_available_modules(sds root);

// Load a single module from path, which should point to a `module.json` file
bool titus_load_module_binary(TitusModule* li);

TitusModule* titus_load_pack_modules(TitusModuleMetaData* required,
                                     size_t required_count,
                                     TitusModule* available,
                                     size_t available_count);

void titus_free_module(TitusModule* module);

// Deserialize a module manifest file to get it's metadata
bool titus_deserialize_manifest(sds path, TitusModuleMetaData* out);

bool titus_meta_data_equal(const TitusModuleMetaData* lhs, const TitusModuleMetaData* rhs);
