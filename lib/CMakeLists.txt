project(lib LANGUAGES CXX)

include(../cmake/config.cmake)

###############################################################################
# library
###############################################################################

# The semantics regarding modules and their use in shared libraries is
# currently quite complicated:
#
#     https://tbekk.com/devstream/blog/2024/04/12/ \
#     c20-modules-cmake-and-shared-libraries/
#
# Until the situation gets a little more clear, `synthclone` headers and module
# files shouldn't be distributed.

add_library(synthclone SHARED)

# config header

configure_file(
    "data/templates/config.h.tmpl"
    "${CMAKE_CURRENT_BINARY_DIR}/include/synthclone/config.h"
)
target_include_directories(
    synthclone
    PUBLIC
    "${CMAKE_CURRENT_BINARY_DIR}/include"
)
file(
    GLOB
    synthclone-lib-headers
    "${CMAKE_CURRENT_BINARY_DIR}/include/synthclone/*.h"
)
# XXX:
#
# install(
#     FILES ${synthclone-lib-headers}
#     DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/synthclone
# )

# modules

# file(GLOB synthclone-lib-core-sources src/core/*.cppm)
file(GLOB synthclone-lib-util-sources src/util/*.cppm)
target_sources(
    synthclone
    PUBLIC
    FILE_SET CXX_MODULES
    TYPE CXX_MODULES
    FILES ${synthclone-lib-util-sources}
)
# XXX:
#
# install(
#     TARGETS synthclone
#     FILE_SET CXX_MODULES
#     DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/synthclone
# )

target_link_libraries(
    synthclone
    samplerate
    sndfile
)

synthclone_configure_target_coverage(synthclone)

###############################################################################
# tests
###############################################################################

add_executable(run-synthclone-lib-tests)
add_test(
    NAME run-synthclone-lib-tests
    COMMAND run-synthclone-lib-tests
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

file(GLOB synthclone-lib-tests tests/util/*.cpp tests/main.cpp)
target_sources(
    run-synthclone-lib-tests
    PUBLIC
    ${synthclone-lib-tests}
)

find_package(
    Boost
    CONFIG
    REQUIRED
    COMPONENTS
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
)
target_link_libraries(
    run-synthclone-lib-tests
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
    kissfft-float
    synthclone
)

synthclone_configure_target_coverage(run-synthclone-lib-tests)
