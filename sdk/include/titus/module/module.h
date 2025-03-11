#pragma once

#include "titus/context/context.h"
#include "titus/sds/sds.h"
#include <SDL3/SDL.h>
#include <stddef.h>
#include <stdint.h>

typedef struct TitusModuleVersion {
    uint64_t major;
    uint64_t minor;
    uint64_t patch;
    sds annotation;
} TitusModuleVersion;

typedef struct TitusModuleManifest {
    sds namespace;
    sds name;
    TitusModuleVersion version;
    sds binary; // Filename of binary without extension
} TitusModuleManifest;

typedef struct TitusModuleMetaData {
    sds namespace;
    sds name;
    TitusModuleVersion version;
} TitusModuleMetaData;

typedef struct TitusModuleLoadInfo {
    TitusModuleManifest manifest;
    sds binary;    // Relative path to binary
    sds resources; // Relative path to resources directory
    sds base;      // Module base directory
} TitusModuleLoadInfo;

typedef void (*titus_initialize_proc)(const TitusApplicationContext*);
typedef void (*titus_deinitialize_proc)(const TitusApplicationContext*);

typedef struct TitusModule {
    TitusModuleManifest manifest;
    SDL_SharedObject* handle;
    sds directory_path;
    sds binary_path;
    sds resource_path;

    titus_initialize_proc initialize;
    titus_deinitialize_proc deinitialize;
} TitusModule;

typedef struct TitusModuleHashmapKV {
    char* key;
    TitusModule value;
} TitusModuleHashmapKV;
