#include "titus/sdk.h"

sds titus_version_to_string(const TitusVersion* ver) {
    TITUS_ASSERT(ver != NULL); // precondition: Attempting to read from NULL

    sds str = sdsempty();
    if(ver->annotation != NULL && sdslen(ver->annotation) > 0) {
        str = sdscatfmt(str, "%U.%U.%U-%s", ver->major, ver->minor, ver->patch, ver->annotation);
    } else {
        str = sdscatfmt(str, "%U.%U.%U", ver->major, ver->minor, ver->patch);
    }

    return str;
}
