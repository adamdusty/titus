#include <assert.h>
#include <string.h>
#include <titus/sdk.h>

void test_titus_version_compare(void) {
    TitusVersion v1 = {0, 1, 0, NULL};
    TitusVersion v2 = {0, 1, 0, NULL};

    int32_t res = titus_version_compare(&v1, &v2);
    int32_t exp = 0;

    assert(res == exp);

    v1 = (TitusVersion){1, 0, 0, NULL};
    v2 = (TitusVersion){0, 1, 0, NULL};

    res = titus_version_compare(&v1, &v2);
    exp = -1;

    assert(res == exp);

    v1 = (TitusVersion){0, 1, 0, NULL};
    v2 = (TitusVersion){1, 0, 0, NULL};

    res = titus_version_compare(&v1, &v2);
    exp = 1;

    assert(res == exp);
}

int main() {
    test_titus_version_compare();
    return 0;
}
