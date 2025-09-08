project(synthclone.util LANGUAGES CXX)

include(../../common/cmake/config.cmake)

###############################################################################
# library
###############################################################################

add_library(synthclone_util SHARED)

# config header
configure_file(
    "data/templates/config.h.tmpl"
    "${CMAKE_CURRENT_BINARY_DIR}/include/synthclone/config.h"
)
target_include_directories(
    synthclone_util
    PUBLIC
    "${CMAKE_CURRENT_BINARY_DIR}/include"
)

# module
file(GLOB synthclone-util-sources src/*.cppm)
target_sources(
    synthclone_util
    PUBLIC
    FILE_SET CXX_MODULES
    TYPE CXX_MODULES
    FILES ${synthclone-util-sources}
)

target_link_libraries(synthclone_util utf8proc)

synthclone_configure_library_target(synthclone_util)

###############################################################################
# tests
###############################################################################

add_executable(run-synthclone-util-tests)
add_dependencies(run-synthclone-util-tests synthclone_test synthclone_util)
add_test(
    NAME run-synthclone-util-tests
    COMMAND run-synthclone-util-tests
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

# tests
file(GLOB synthclone-util-tests tests/*.cpp)
target_sources(
    run-synthclone-util-tests
    PUBLIC
    ${synthclone-util-tests}
)

find_package(
    Boost
    CONFIG
    REQUIRED
    COMPONENTS
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
)
target_link_libraries(
    run-synthclone-util-tests
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
    synthclone_test
    synthclone_util
)

synthclone_configure_test_target(run-synthclone-util-tests)
