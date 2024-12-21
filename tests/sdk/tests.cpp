#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

TEST_CASE("Hello runtime") {
    int* x = new int(24);
    delete x;

    CHECK(*x == 24);
}