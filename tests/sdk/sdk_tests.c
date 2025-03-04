#include <assert.h>
#include <stdlib.h>
#include <titus/sdk.h>

bool mat4f_equal(const mat4f* lhs, const mat4f* rhs) {
    return 0 == memcmp(lhs, rhs, sizeof(mat4f));
}

void test_matrix_multiply() {
    mat4f a = {
        .w = {1.0f, 0.0f, 0.0f},
        .x = {0.0f, 1.0f, 0.0f},
        .y = {0.0f, 0.0f, 1.0f},
        .z = {0.0f, 0.0f, 0.0f},
    };
    mat4f b = {
        .w = {2.0f, 0.0f, 0.0f},
        .x = {0.0f, 2.0f, 0.0f},
        .y = {0.0f, 0.0f, 2.0f},
        .z = {0.0f, 0.0f, 0.0f},
    };

    mat4f result   = titus_mat_mul(&a, &b);
    mat4f expected = {
        .w = {2.0f, 0.0f, 0.0f},
        .x = {0.0f, 2.0f, 0.0f},
        .y = {0.0f, 0.0f, 2.0f},
        .z = {0.0f, 0.0f, 0.0f},
    };

    assert(false);
}

int main() {
    test_matrix_multiply();
}