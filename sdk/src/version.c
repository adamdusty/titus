#include "titus/version.h"

#include "titus/assert.h"
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
TITUS_EXPORT bool titus_parse_version(const char* version_str, TitusVersion* out) {
    TITUS_ASSERT(version_str != NULL); // Can't parse a non-existent string

    int num_splits = 0;
    sds* splits    = sdssplitlen(version_str, (ptrdiff_t)strlen(version_str), "-", 1, &num_splits);
    if(num_splits == 2) {
        out->annotation = splits[1];
    }

    num_splits    = 0;
    sds* versions = sdssplitlen(splits[0], (ptrdiff_t)sdslen(splits[0]), ".", 1, &num_splits);
    if(num_splits != 3) {
        return false; // Version requires major, minor, and patch
    }

    errno      = 0;
    char* end  = NULL;
    out->major = strtoull(versions[0], &end, 10);
    if(errno == ERANGE || end == versions[0]) {
        return false;
    }

    errno      = 0;
    end        = NULL;
    out->minor = strtoull(versions[1], &end, 10);
    if(errno == ERANGE || end == versions[1]) {
        return false;
    }

    errno      = 0;
    end        = NULL;
    out->patch = strtoull(versions[2], &end, 10);
    if(errno == ERANGE || end == versions[2]) {
        return false;
    }

    return true;
}

bool titus_version_satisfies_caret(const TitusVersion* ver, const TitusVersion* req) {
    return false;
}

bool titus_version_satisfies_patch(const TitusVersion* ver, const TitusVersion* req) {
    return false;
}

bool titus_version_satifies(const TitusVersion* ver, const TitusVersion* req, const char* op) {
    char first, second;
    /* Extract the comparison operator */
    first  = op[0];
    second = op[1];

    /* Caret operator */
    if(first == '^')
        return titus_version_satisfies_caret(ver, req);

    /* Tilde operator */
    if(first == '~')
        return titus_version_satisfies_patch(ver, req);

    /* Strict equality */
    if(first == '=')
        return titus_version_compare(ver, req) == 0;

    /* Greater than or equal comparison */
    if(first == '>') {
        if(second == '=') {
            return titus_version_compare(ver, req) >= 0;
        }
        return titus_version_compare(ver, req) > 0;
    }

    /* Lower than or equal comparison */
    if(first == '<') {
        if(second == '=') {
            return titus_version_compare(ver, req) <= 0;
        }
        return titus_version_compare(ver, req) < 0;
    }

    return false;
}
