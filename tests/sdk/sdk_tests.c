#include <assert.h>
#include <string.h>
#include <titus/sdk.h>

bool mat4f_equal(const mat4f* lhs, const mat4f* rhs) {
    return 0 == memcmp(lhs, rhs, sizeof(mat4f));
}

void test_matrix_multiply() {
    mat4f a = {
        .w = {1.0f, 0.0f, 0.0f, 0.0f},
        .x = {0.0f, 1.0f, 0.0f, 0.0f},
        .y = {0.0f, 0.0f, 1.0f, 0.0f},
        .z = {0.0f, 0.0f, 0.0f, 0.0f},
    };
    mat4f b = {
        .w = {2.0f, 0.0f, 0.0f, 0.0f},
        .x = {0.0f, 2.0f, 0.0f, 0.0f},
        .y = {0.0f, 0.0f, 2.0f, 0.0f},
        .z = {0.0f, 0.0f, 0.0f, 0.0f},
    };

    mat4f res = titus_mat_mul(&a, &b);
    mat4f exp = {
        .w = {2.0f, 0.0f, 0.0f},
        .x = {0.0f, 2.0f, 0.0f},
        .y = {0.0f, 0.0f, 2.0f},
        .z = {0.0f, 0.0f, 0.0f},
    };

    assert(mat4f_equal(&res, &exp));
}

void test_matrix_translate() {
    vec3f trans = {.x = 4.0f, .y = 5.0f, .z = 6.0f};
    mat4f mat   = {
          .w = {1.0f, 2.0f, 3.0f},
          .x = {1.0f, 0.0f, 0.0f},
          .y = {0.0f, 1.0f, 0.0f},
          .z = {0.0f, 0.0f, 1.0f},
    };

    mat4f res = titus_mat_translate(&mat, &trans);
    mat4f exp = {
        .w = {5.0f, 7.0f, 9.0f, 0.0f},
        .x = {1.0f, 0.0f, 0.0f, 0.0f},
        .y = {0.0f, 1.0f, 0.0f, 0.0f},
        .z = {0.0f, 0.0f, 1.0f, 0.0f},
    };

    assert(mat4f_equal(&res, &exp));
}

int main() {
    test_matrix_multiply();
    test_matrix_translate();
}