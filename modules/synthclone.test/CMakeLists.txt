project(synthclone.test LANGUAGES CXX)

include(../../common/cmake/config.cmake)

###############################################################################
# library
###############################################################################

add_library(synthclone_test OBJECT)
add_dependencies(
    synthclone_test
    synthclone_core
    synthclone_external_boost
    synthclone_util
)

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
    PRIVATE
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
    synthclone_core
    synthclone_external_boost
    synthclone_external_posix
    synthclone_external_samplerate
    synthclone_external_sndfile
    synthclone_external_utf8proc
    synthclone_util
)

synthclone_configure_internal_library_target(synthclone_test)
