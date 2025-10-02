project(synthclone.qt LANGUAGES CXX)

include(../../common/cmake/config.cmake)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

###############################################################################
# library
###############################################################################

add_library(synthclone_qt OBJECT)
add_dependencies(
    synthclone_qt
    synthclone_external_boost
    synthclone_external_qt
    synthclone_util
)

# module
file(GLOB synthclone-qt-sources src/*.cppm)
target_sources(
    synthclone_qt
    PUBLIC
    FILE_SET CXX_MODULES
    TYPE CXX_MODULES
    FILES ${synthclone-qt-sources}
)

target_include_directories(
    synthclone_qt
    PUBLIC
    "${CMAKE_CURRENT_BINARY_DIR}/include"
)

target_link_libraries(
    synthclone_qt
    PRIVATE
    synthclone_external_boost
    synthclone_external_posix
    synthclone_external_qt
    synthclone_external_utf8proc
    synthclone_util
)

synthclone_configure_internal_library_target(synthclone_qt)

###############################################################################
# tests
###############################################################################

add_executable(run-synthclone-qt-tests)
add_dependencies(
    run-synthclone-qt-tests
    synthclone_external_qt
    synthclone_qt
    synthclone_qt_test
    synthclone_test
    synthclone_util
)
add_test(
    NAME run-synthclone-qt-tests
    COMMAND run-synthclone-qt-tests
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

# tests
file(GLOB synthclone-qt-tests tests/*.cpp)
target_sources(
    run-synthclone-qt-tests
    PUBLIC
    ${synthclone-qt-tests}
)

find_package(
    Boost
    CONFIG
    REQUIRED
    COMPONENTS
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
)
target_link_libraries(
    run-synthclone-qt-tests
    PRIVATE
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
    synthclone_external_boost
    synthclone_external_posix
    synthclone_external_qt
    synthclone_external_utf8proc
    synthclone_qt
    synthclone_qt_test
    synthclone_test
    synthclone_util
    boost_unit_test_framework
)

synthclone_configure_test_target(run-synthclone-qt-tests)
