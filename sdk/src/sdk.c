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

/**
 * @brief Compare two semantic versions, ignoring annotation. Returns -1 if lhs > rhs, 1 if rhs > lhs, and 0 if versions
 * are equal.
 *
 * @param lhs
 * @param rhs
 * @return -1 if lhs > rhs, 1 if rhs > lhs, and 0 if versions are equal
 */
TITUS_EXPORT int32_t titus_version_compare(const TitusVersion* lhs, const TitusVersion* rhs) {
    if(lhs->major == rhs->major && lhs->minor == rhs->minor && lhs->patch == rhs->patch) {
        return 0; // Versions are equal
    }

    if(lhs->major > rhs->major) {
        return -1;
    } else if(rhs->major > lhs->major) {
        return 1;
    } else { // Major versions are equal
        if(lhs->minor > rhs->minor) {
            return -1;
        } else if(rhs->minor > lhs->minor) {
            return 1;
        } else {
            if(lhs->patch > rhs->patch) {
                return -1;
            } else if(rhs->patch > lhs->patch) {
                return 1;
            }
        }
    }

    unreachable();
}

TITUS_EXPORT TitusVersion titus_parse_version(const char* version_str) {
    TITUS_ASSERT(false); // Unimplemented
}