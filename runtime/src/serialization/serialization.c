#include "serialization/serialization.h"

/**
 * @brief Deserializes a json object and returns the result. If no error occured, the error field will be NULL,
 * otherwise it will contain a message describing the error. If `error` is not `NULL`, then `value` is not completely
 * defined.
 *
 * @param json
 * @return TitusModulePackDeserializationResult
 */
TitusModulePackDeserializationResult titus_module_pack_deserialize(yyjson_val* json) {
    TITUS_ASSERT(json != NULL); // precondition: json data is not null

    TitusModulePackDeserializationResult result = {0};

    if(yyjson_get_type(json) != YYJSON_TYPE_OBJ) {
        result.error = "JSON value for pack should be of type object.";
        return result;
    }

    yyjson_val* jname = yyjson_obj_get(json, "name");
    if(jname == NULL) {
        result.error = "Required field name missing from JSON object.";
        return result;
    }

    yyjson_val* jmodules = yyjson_obj_get(json, "modules");
    if(jmodules == NULL) {
        result.error = "Required field modules not found.";
        return result;
    } else if(yyjson_get_type(jmodules) != YYJSON_TYPE_ARR) {
        result.error = "modules field should an array.";
        return result;
    }

    result.pack.name         = sdsnew(yyjson_get_str(jname));
    result.pack.module_count = yyjson_arr_size(jmodules);
    result.pack.modules      = malloc(sizeof(TitusModuleMetaData) * result.pack.module_count);
    if(result.pack.modules == NULL) {
        titus_log_error("Failed to allocate memory: %s:%d [%s]", __FILE__, __LINE__, __func__);
        titus_log_error("Exiting application.");
        exit(EXIT_FAILURE);
    }

    yyjson_val* hit = NULL;
    size_t idx      = 0;
    size_t max      = 0;
    yyjson_arr_foreach(jmodules, idx, max, hit) {
        TitusRequiredModuleDeserializationResult module_result = titus_required_module_deserialize(hit);
        if(module_result.error != NULL) {
            result.error = module_result.error;
            return result;
        }

        result.pack.modules[idx] = module_result.module;
    }

    return result;
}

TitusModuleMetaDataDeserializationResult titus_module_meta_deserialize(yyjson_val* json) {
    TitusModuleMetaDataDeserializationResult result = {0};

    if(yyjson_get_type(json) != YYJSON_TYPE_OBJ) {
        result.error = "JSON value for module metadata should be object.";
        return result;
    }

    yyjson_val* jnamespace = yyjson_obj_get(json, "namespace");
    if(jnamespace == NULL) {
        result.error = "Required field `namespace` not found.";
        return result;
    } else if(yyjson_get_type(jnamespace) != YYJSON_TYPE_STR) {
        result.error = "JSON value for field `namespace` should a string.";
        return result;
    }

    yyjson_val* jname = yyjson_obj_get(json, "name");
    if(jname == NULL) {
        result.error = "Required field `name` not found.";
        return result;
    } else if(yyjson_get_type(jname) != YYJSON_TYPE_STR) {
        result.error = "JSON value for field `jname` should a string.";
        return result;
    }

    yyjson_val* jversion = yyjson_obj_get(json, "version");
    if(jversion == NULL) {
        result.error = "Required field `version` not found.";
        return result;
    } else if(yyjson_get_type(jversion) != YYJSON_TYPE_STR) {
        result.error = "JSON value for field `jversion` should a string.";
        return result;
    }

    result.meta.namespace = sdsnew(yyjson_get_str(jnamespace));
    result.meta.name      = sdsnew(yyjson_get_str(jname));
    if(!titus_parse_version(yyjson_get_str(jversion), &result.meta.version)) {
        result.error = "Failed to parse version number.";
        return result;
    }

    return result;
}

TitusRequiredModuleDeserializationResult titus_required_module_deserialize(yyjson_val* json) {
    TitusRequiredModuleDeserializationResult result = {0};

    if(yyjson_get_type(json) != YYJSON_TYPE_OBJ) {
        result.error = "JSON value for required module should be object.";
        return result;
    }

    yyjson_val* jnamespace = yyjson_obj_get(json, "namespace");
    if(jnamespace == NULL) {
        result.error = "Required field `namespace` not found.";
        return result;
    } else if(yyjson_get_type(jnamespace) != YYJSON_TYPE_STR) {
        result.error = "JSON value for field `namespace` should a string.";
        return result;
    }

    yyjson_val* jname = yyjson_obj_get(json, "name");
    if(jname == NULL) {
        result.error = "Required field `name` not found.";
        return result;
    } else if(yyjson_get_type(jname) != YYJSON_TYPE_STR) {
        result.error = "JSON value for field `jname` should a string.";
        return result;
    }

    yyjson_val* jversion = yyjson_obj_get(json, "version");
    if(jversion == NULL) {
        result.error = "Required field `version` not found.";
        return result;
    } else if(yyjson_get_type(jversion) != YYJSON_TYPE_STR) {
        result.error = "JSON value for field `jversion` should a string.";
        return result;
    }

    result.module.nspace = sdsnew(yyjson_get_str(jnamespace));
    result.module.name   = sdsnew(yyjson_get_str(jname));
    if(!titus_parse_version(yyjson_get_str(jversion), &result.module.version)) {
        result.error = "Failed to parse version number.";
        return result;
    }

    return result;
}
