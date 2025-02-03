if(TITUS_FETCHCONTENT)

include(FetchContent)

FetchContent_Declare(
    SDL3
    GIT_REPOSITORY  https://github.com/libsdl-org/SDL.git
    GIT_TAG         release-3.2.0
    GIT_SHALLOW
    SYSTEM
    EXCLUDE_FROM_ALL
    FIND_PACKAGE_ARGS
)

FetchContent_Declare(
    flecs
    GIT_REPOSITORY  https://github.com/SanderMertens/flecs.git
    GIT_TAG         v4.0.4
    GIT_SHALLOW
    SYSTEM
    FIND_PACKAGE_ARGS
)

FetchContent_MakeAvailable(SDL3 flecs)

endif(TITUS_FETCHCONTENT)