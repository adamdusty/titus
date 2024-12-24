include(FetchContent)

FetchContent_Declare(
    flecs
    GIT_REPOSITORY https://github.com/SanderMertens/flecs.git
    GIT_TAG        v4.0.3
    SYSTEM
)

FetchContent_Declare(
    quill
    GIT_REPOSITORY https://github.com/odygrd/quill.git
    GIT_TAG        v7.5.0
    SYSTEM
)

FetchContent_MakeAvailable(flecs quill)