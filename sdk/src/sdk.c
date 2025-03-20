#include "titus/sdk.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>

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

/**
 * @brief Parse a version string. Expected to be of form `"major.minor.patch[-annotation]". Overwrites memory at `out`
 * address. On failure, `out` is only partially initialized.
 *
 * @param version_str
 * @param out Address where the version is written too.
 * @return `true` on success, `false` on error.
 */
TITUS_EXPORT bool titus_parse_version(char* version_str, TitusVersion* out) {
    TITUS_ASSERT(version_str != NULL); // Can't parse a non-existent string

    sds version = sdsnew(version_str);

    char* token  = NULL;
    char* rest   = NULL;
    char* endptr = NULL;

    // Parse major version
    token = strtok_s(version, ".", &rest);
    if(token == NULL) {
        sdsfree(version);
        return false;
    }
    errno      = 0; // Reset errno before each call
    out->major = strtoull(token, &endptr, 10);
    if(errno == ERANGE || token == endptr || *endptr != '\0') {
        sdsfree(version);
        return false;
    }

    // Parse minor version
    token = strtok_s(NULL, ".", &rest);
    if(token == NULL) {
        sdsfree(version);
        return false;
    }
    errno      = 0;
    out->minor = strtoull(token, &endptr, 10);
    if(errno == ERANGE || token == endptr || *endptr != '\0') {
        sdsfree(version);
        return false;
    }

    // Parse patch version and optional annotation
    token = strtok_s(NULL, "-", &rest);
    if(token == NULL) {
        sdsfree(version);
        return false;
    }
    errno      = 0;
    out->patch = strtoull(token, &endptr, 10);
    if(errno == ERANGE || token == endptr || *endptr != '\0') {
        sdsfree(version);
        return false;
    }

    // Parse annotation (if present)
    token = strtok_s(NULL, "", &rest);
    if(token != NULL) {
        out->annotation = strdup(token);
        if(out->annotation == NULL) {
            sdsfree(version);
            return false;
        }
    }

    sdsfree(version);
    return true;
}
