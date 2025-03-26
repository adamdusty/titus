#include "module/module.h"
#include "serialization/serialization.h"

#include <stdio.h>
#include <yyjson.h>

#define test_assert(expr)                                                                                              \
    if(!(expr)) {                                                                                                      \
        fprintf(stderr, "Test failed [%s] %s:%d", __func__, __FILE__, __LINE__);                                       \
        exit(EXIT_FAILURE);                                                                                            \
    }

void test_deserialize_module_metadata(void) {
    const char json[] = {
#embed "resources/metadata.json"
    };

    yyjson_doc* doc  = yyjson_read(json, sizeof(json), YYJSON_READ_ALLOW_COMMENTS | YYJSON_READ_ALLOW_TRAILING_COMMAS);
    yyjson_val* root = yyjson_doc_get_root(doc);

    TitusModuleMetaData exp = {
        .namespace = sdsnew("core"),
        .name      = sdsnew("input"),
        .version =
            {
                .major      = 0,
                .minor      = 1,
                .patch      = 0,
                .annotation = sdsnew("alpha"),
            },
    };

    TitusModuleMetaDataDeserializationResult res = titus_module_meta_deserialize(root);
    yyjson_doc_free(doc);

    test_assert(res.error == NULL);
    test_assert(titus_meta_data_equal(&res.meta, &exp));
}

void test_required_module_deserializeation(void) {

    const char json[] = {
#embed "resources/required_module.json"
    };

    yyjson_doc* doc  = yyjson_read(json, sizeof(json), YYJSON_READ_ALLOW_COMMENTS | YYJSON_READ_ALLOW_TRAILING_COMMAS);
    yyjson_val* root = yyjson_doc_get_root(doc);

    TitusRequiredModule exp = {
        .nspace  = sdsnew("core"),
        .name    = sdsnew("input"),
        .version = {0, 1, 0},
    };

    TitusRequiredModuleDeserializationResult res = titus_required_module_deserialize(root);
    yyjson_doc_free(doc);

    test_assert(res.error == NULL);
    test_assert(sdscmp(res.module.nspace, exp.nspace) == 0);
    test_assert(sdscmp(res.module.name, exp.name) == 0);
    test_assert(titus_version_compare(&res.module.version, &exp.version) == 0);
}

int main() {
    test_deserialize_module_metadata();
    test_required_module_deserializeation();
    return 0;
}