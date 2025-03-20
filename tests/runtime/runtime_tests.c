#include "module/module.h"
#include "serialization/serialization.h"

#include <stdio.h>
#include <yyjson.h>

#define test_assert(expr)                                                                                              \
    if(!(expr)) {                                                                                                      \
        fprintf(stderr, "Test failed %s:%d", __FILE__, __LINE__);                                                      \
        exit(EXIT_FAILURE);                                                                                            \
    }

void test_deserialize_module_metadata(void) {
    const char json[] = {
#embed "metadata.json"
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
    test_assert(res.error == NULL);
    test_assert(titus_meta_data_equal(&res.meta, &exp));
}

int main() {
    test_deserialize_module_metadata();
    return 0;
}