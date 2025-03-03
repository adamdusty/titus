if(TITUS_FETCHCONTENT)

include(FetchContent)

FetchContent_Declare(
    SDL3
    GIT_REPOSITORY  https://github.com/libsdl-org/SDL.git
    GIT_TAG         release-3.2.4
    GIT_SHALLOW
    SYSTEM
    EXCLUDE_FROM_ALL
)

FetchContent_Declare(
    cglm
    GIT_REPOSITORY  https://github.com/recp/cglm.git
    GIT_TAG         v0.9.6
    GIT_SHALLOW
    SYSTEM
    EXCLUDE_FROM_ALL
)

FetchContent_Declare(
    flecs
    GIT_REPOSITORY  https://github.com/SanderMertens/flecs.git
    GIT_TAG         v4.0.4
    GIT_SHALLOW
    SYSTEM
    FIND_PACKAGE_ARGS
)

FetchContent_Declare(
    yyjson
    GIT_REPOSITORY  https://github.com/ibireme/yyjson.git
    GIT_TAG         0.10.0
    GIT_SHALLOW
    SYSTEM
    FIND_PACKAGE_ARGS
)


FetchContent_MakeAvailable(SDL3 flecs yyjson cglm)

endif(TITUS_FETCHCONTENT)