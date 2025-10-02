project(synthclone.core LANGUAGES CXX)

include(../../common/cmake/config.cmake)

###############################################################################
# library
###############################################################################

add_library(synthclone_core OBJECT)
add_dependencies(
    synthclone_core
    synthclone_external_boost
    synthclone_external_samplerate
    synthclone_external_sndfile
    synthclone_util
)

# project version information submodule
configure_file(
    "data/templates/project.cppm.tmpl"
    "${CMAKE_CURRENT_BINARY_DIR}/modules/project.cppm"
)

# module
file(
    GLOB
    synthclone-core-sources
    src/*.cppm
    ${CMAKE_CURRENT_BINARY_DIR}/modules/*.cppm
)
target_sources(
    synthclone_core
    PUBLIC
    FILE_SET CXX_MODULES
    TYPE CXX_MODULES
    BASE_DIRS
    src
    ${CMAKE_CURRENT_BINARY_DIR}/modules
    FILES
    ${synthclone-core-sources}
)

target_include_directories(
    synthclone_core
    PUBLIC
    "${CMAKE_CURRENT_BINARY_DIR}/include"
)

target_link_libraries(
    synthclone_core
    PRIVATE
    synthclone_external_boost
    synthclone_external_posix
    synthclone_external_samplerate
    synthclone_external_sndfile
    synthclone_external_utf8proc
    synthclone_util
)

synthclone_configure_internal_library_target(synthclone_core)

###############################################################################
# tests
###############################################################################

add_executable(run-synthclone-core-tests)
add_dependencies(
    run-synthclone-core-tests
    synthclone_core
    synthclone_core_test
    synthclone_external_boost
    synthclone_external_kissfft
    synthclone_test
    synthclone_util
)
add_test(
    NAME run-synthclone-core-tests
    COMMAND run-synthclone-core-tests
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

# module
file(GLOB synthclone-core-tests-module tests/*.cppm)
target_sources(
    run-synthclone-core-tests
    PUBLIC
    FILE_SET CXX_MODULES
    TYPE CXX_MODULES
    FILES ${synthclone-core-tests-module}
)

# tests
file(GLOB synthclone-core-tests tests/*.cpp)
target_sources(
    run-synthclone-core-tests
    PUBLIC
    ${synthclone-core-tests}
)

find_package(
    Boost
    CONFIG
    REQUIRED
    COMPONENTS
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
)
target_link_libraries(
    run-synthclone-core-tests
    PRIVATE
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
    synthclone_core
    synthclone_core_test
    synthclone_external_boost
    synthclone_external_kissfft
    synthclone_external_posix
    synthclone_external_samplerate
    synthclone_external_sndfile
    synthclone_external_utf8proc
    synthclone_test
    synthclone_util
)

synthclone_configure_test_target(run-synthclone-core-tests)
