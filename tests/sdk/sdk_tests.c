#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <titus/sdk.h>

#define test_assert(expr)                                                                                              \
    if(!(expr)) {                                                                                                      \
        fprintf(stderr, "Test failed %s:%d", __FILE__, __LINE__);                                                      \
        exit(EXIT_FAILURE);                                                                                            \
    }

void test_titus_version_compare(void) {
    TitusVersion v1 = {0, 1, 0, NULL};
    TitusVersion v2 = {0, 1, 0, NULL};

    int32_t res = titus_version_compare(&v1, &v2);
    int32_t exp = 0;

    test_assert(res == exp);

    v1 = (TitusVersion){1, 0, 0, NULL};
    v2 = (TitusVersion){0, 1, 0, NULL};

    res = titus_version_compare(&v1, &v2);
    exp = -1;

    test_assert(res == exp);

    v1 = (TitusVersion){0, 1, 0, NULL};
    v2 = (TitusVersion){1, 0, 0, NULL};

    res = titus_version_compare(&v1, &v2);
    exp = 1;

    test_assert(res == exp);
}

void test_parse_version(void) {
    char* version = "1.2.3-alpha";

    TitusVersion res = {0};
    TitusVersion exp = {.major = 1, .minor = 2, .patch = 3, .annotation = sdsnew("alpha")};

    bool parse_result = titus_parse_version(version, &res);
    test_assert(parse_result);
    test_assert(titus_version_compare(&res, &exp) == 0);

    version = "0.1.0";

    parse_result = titus_parse_version(version, &res);
    test_assert(parse_result);

    exp = (TitusVersion){.major = 0, .minor = 1, .patch = 0, .annotation = NULL};
    test_assert(titus_version_compare(&res, &exp) == 0);

    version = "75423.1.19836-betalphaomega";

    parse_result = titus_parse_version(version, &res);
    test_assert(parse_result);

    exp = (TitusVersion){.major = 75423, .minor = 1, .patch = 19836, .annotation = sdsnew("betalphaomega")};
    test_assert(titus_version_compare(&res, &exp) == 0);

    version = "incorrectversionfmt";

    parse_result = titus_parse_version(version, &res);
    test_assert(parse_result == false);
}

int main() {
    test_titus_version_compare();
    test_parse_version();
    return 0;
}
