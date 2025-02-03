/*
TODO:
    - Make powershell script for integration testing
*/

#include "config/config.h"
#include "plugin/plugin.h"
#include <stddef.h>

#define CONFIG_PATH "undefined"

int main(int argc, char* argv[]) {
    titus_config app_config = titus_load_config(CONFIG_PATH);

    titus_plugin_array plugins = titus_plugin_array_new();
    titus_result result        = load_plugins(&plugins, "build/dev/runtime/plugins");

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