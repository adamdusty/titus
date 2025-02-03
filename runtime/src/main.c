#include "config/config.h"
#include "plugin/plugin.h"
#include "sds/sds.h"

#include <SDL3/SDL.h>
#include <stddef.h>
#include <stdio.h>

#define CONFIG_PATH "undefined"

int main(int argc, char* argv[]) {
    titus_config app_config = titus_load_config(CONFIG_PATH);

    sds executable_directory_path = sdsnew(SDL_GetBasePath());
    sds plugin_path               = sdsempty();
    plugin_path                   = sdscat(plugin_path, executable_directory_path);
    plugin_path                   = sdscat(plugin_path, app_config.plugin_root_directory);

    titus_plugin_array plugins = titus_plugin_array_new();
    titus_result result        = load_plugins(&plugins, plugin_path);
    if(TITUS_RESULT_OK != result.type) {
        fprintf(stderr, "%s", result.msg);
        return result.type;
    }

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

    return 0;
}