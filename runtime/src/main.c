#include "config/config.h"
#include "ds/stb_ds.h"
#include "log/log.h"
#include "module/module.h"
#include "sds/sds.h"

#include <SDL3/SDL.h>
#include <stddef.h>
#include <stdio.h>

#define CONFIG_PATH "undefined"

// void titus_log_function(void* ud, int cat, SDL_LogPriority pri, const char* msg) {
//     SDL_IOStream* s = (SDL_IOStream*)ud;
//     if(NULL != s) {
//         SDL_WriteIO(s, msg, SDL_strlen(msg));
//     }
// }

int main(int, char*[]) {
    // SDL_IOStream* log = SDL_IOFromFile("log.txt", "a");
    // SDL_SetLogOutputFunction(titus_log_function, log);

    titus_config app_config = titus_load_config(CONFIG_PATH);

    sds executable_directory_path = sdsnew(SDL_GetBasePath());
    sds module_path               = sdsempty();
    module_path                   = sdscat(module_path, executable_directory_path);
    module_path                   = sdscat(module_path, app_config.module_root_directory);

    sds* manifest_paths = titus_get_manifest_paths_from_root(module_path);

    titus_module_manifest* manifest = NULL;
    for(int i = 0; i < arrlen(manifest_paths); i++) {
        titus_module_manifest man = {0};

        bool res = titus_load_manifest_from_path(manifest_paths[i], &man);
        if(res) {
            arrpush(manifest, man);
            log_info("%s:%s", man.namespace, man.name);
        }
    }

    // titus_module* modules = NULL;
    // titus_result result   = load_modules(modules, module_path);
    // if(TITUS_RESULT_OK != result.type) {
    //     fprintf(stderr, "%s", result.msg);
    //     return result.type;
    // }

    // for(ptrdiff_t i = 0; i < plugs.count; i++) {
    //     plugs[i].initialize(world);
    // }

    // timer t = timer_new();

    // bool quit = false;
    // while(!quit) {
    //     ecs_progress(world, t.delta);
    // }

    // for(ptrdiff_t i = 0; i < plugs.count; i++) {
    //     plugs[i].deinitialize(world);
    // }

    /* Clean up */
    for(int i = 0; i < arrlen(manifest_paths); i++) {
        sdsfree(manifest_paths[i]);
    }
    arrfree(manifest_paths);
    /* ***** */

    return 0;
}
