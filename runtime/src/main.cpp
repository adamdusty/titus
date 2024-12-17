#include <iostream>
#include <titus/sdk.hpp>

auto main() -> int {
    std::cerr << titus::version() << '\n';
    return 0;
}