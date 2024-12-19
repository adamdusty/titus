project(
    titus-tests
    LANGUAGES CXX
)

include(FetchContent)
FetchContent_Declare(
    doctest
    GIT_REPOSITORY https://github.com/doctest/doctest.git
    GIT_TAG        master
    SYSTEM
)
FetchContent_MakeAvailable(doctest)

add_executable(sdk_tests)
add_executable(rt_tests)

target_link_libraries(sdk_tests PRIVATE doctest::doctest)
target_link_libraries(rt_tests PRIVATE doctest::doctest)

target_sources(sdk_tests PRIVATE sdk/tests.cpp)
target_sources(rt_tests PRIVATE rt/tests.cpp)

add_test(NAME sdk_tests COMMAND sdk_tests)
add_test(NAME rt_tests COMMAND rt_tests)

if(WIN32)

set(SDK_DLL_DIRS "$<TARGET_RUNTIME_DLL_DIRS:sdk_tests>")
set(ENV_OP "PATH=path_list_append:")

set_tests_properties(sdk_tests PROPERTIES
    ENVIRONMENT_MODIFICATION
        "${ENV_OP}$<JOIN:${SDK_DLL_DIRS},$<SEMICOLON>${ENV_OP}>"
)

set(RT_DLL_DIRS "$<TARGET_RUNTIME_DLL_DIRS:rt_tests>")
set_tests_properties(rt_tests PROPERTIES
    ENVIRONMENT_MODIFICATION
        "${ENV_OP}$<JOIN:${RT_DLL_DIRS},$<SEMICOLON>${ENV_OP}>"
)

endif()

set(TEST_COMPILE_OPTS
    $<$<CONFIG:Release>:-U_FORTIFY_SOURCE>
    $<$<CONFIG:Release>:-D_FORTIFY_SOURCE=3>
    -fno-omit-frame-pointer
    -fstack-protector-strong
    -fcf-protection=full
    -Wall
    -Wextra
    -Wpedantic
    -Wconversion
    -Wsign-conversion
    -Wcast-qual
    -Wformat=2
    -Wundef
    -Werror=float-equal
    -Wcast-align
    -Wunused
    -Wnull-dereference
    -Wdouble-promotion
    -Wimplicit-fallthrough
    -Wextra-semi
    -Woverloaded-virtual
    -Wnon-virtual-dtor
    -Wold-style-cast
)

target_compile_options(sdk_tests PRIVATE ${TEST_COMPILE_OPTS})
target_compile_options(rt_tests PRIVATE ${TEST_COMPILE_OPTS})