project(synthclone.test LANGUAGES CXX)

include(../../common/cmake/config.cmake)

###############################################################################
# library
###############################################################################

add_library(synthclone_test SHARED)
add_dependencies(synthclone_test synthclone_core synthclone_util)

# module

file(GLOB synthclone-test-sources src/*.cppm)
target_sources(
    synthclone_test
    PUBLIC
    FILE_SET CXX_MODULES
    TYPE CXX_MODULES
    FILES ${synthclone-test-sources}
)

find_package(
    Boost
    CONFIG
    REQUIRED
    COMPONENTS
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
)
target_link_libraries(
    synthclone_test
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
    synthclone_core
    synthclone_util
)

synthclone_configure_target_coverage(synthclone_test)
