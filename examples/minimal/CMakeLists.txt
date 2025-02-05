add_library(minimal SHARED src/mod.c)

set(MINIMAL_NSPACE "example")
set(MINIMAL_NAME "minimal")
set(MINIMAL_VERSION "1.6.42656-alpha")
set(MINIMAL_DIR "${MINIMAL_NSPACE}/${MINIMAL_NAME}/${MINIMAL_VERSION}" PARENT_SCOPE)