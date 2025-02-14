#include "titus/assets/assets.h"

#include "titus/ds/stb_ds.h"
#include "titus/log/log.h"
#include "titus/module/module.h"


char* titus_get_asset_path(ecs_world_t* ecs, char* namespace, char* name, char* /* path */) {
    ecs_entity_t module_map_component = ecs_lookup(ecs, "runtime:module_map");
    if(module_map_component == 0) {
        titus_log_error("Unable to find module map");
        return NULL;
    } else {
        titus_log_info("Found module map component");
    }
    titus_log_info("%llu", module_map_component);
    module_kv* mod_map = *(module_kv**)ecs_get_mut_id(ecs, module_map_component, module_map_component);
    titus_log_info("Mod map singleton: %p", (void*)mod_map);

    sds p = sdsempty();
    p     = sdscatfmt(p, "%s:%s", namespace, name);

    module_kv* mod = shgetp(mod_map, p);
    titus_log_info("ASSET: %s", mod->value.resource_path);

    return NULL;
}
