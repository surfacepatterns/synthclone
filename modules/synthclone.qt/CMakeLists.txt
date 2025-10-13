project(module-synthclone.qt LANGUAGES CXX)

include(../../common/cmake/config.cmake)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

###############################################################################
# library
###############################################################################

file(GLOB module-synthclone.qt-sources src/*.cppm)

function(synthclone_make_module type)
    set(target "module-synthclone.qt-${type}")
    add_library(${target} OBJECT)
    add_dependencies(
        ${target}
        module-synthclone.external.boost.callable_traits-${type}
        module-synthclone.external.boost.core-${type}
        module-synthclone.external.boost.mp11-${type}
        module-synthclone.external.qt.core-${type}
        module-synthclone.external.qt.widgets-${type}
        module-synthclone.util-${type}
    )
    target_sources(
        ${target}
        PUBLIC
        FILE_SET CXX_MODULES
        TYPE CXX_MODULES
        FILES ${module-synthclone.qt-sources}
    )
    target_link_libraries(
        ${target}
        PRIVATE
        module-synthclone.external.boost.callable_traits-${type}
        module-synthclone.external.boost.core-${type}
        module-synthclone.external.boost.mp11-${type}
        module-synthclone.external.qt.core-${type}
        module-synthclone.external.qt.widgets-${type}
        module-synthclone.util-${type}
    )
    synthclone_configure_module(${target} ${type})
endfunction()

synthclone_make_module(object)
synthclone_make_module(shared)

###############################################################################
# tests
###############################################################################

file(GLOB module-synthclone.qt-tests tests/*.cpp)

function(synthclone_make_tests)
    set(target "run-synthclone-qt-tests")
    add_executable(${target})
    add_dependencies(
        ${target}
        module-synthclone.external.boost.unit_test-object
        module-synthclone.external.qt.core-object
        module-synthclone.external.qt.gui-object
        module-synthclone.external.qt.widgets-object
        module-synthclone.qt-object
        module-synthclone.qt.test-object
        module-synthclone.test-object
        module-synthclone.util-object
    )
    add_test(
        NAME ${target}
        COMMAND ${target}
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )
    target_sources(
        ${target}
        PUBLIC
        ${module-synthclone.qt-tests}
    )
    target_link_libraries(
        ${target}
        PRIVATE
        module-synthclone.external.boost.callable_traits-object
        module-synthclone.external.boost.core-object
        module-synthclone.external.boost.mp11-object
        module-synthclone.external.boost.scope-object
        module-synthclone.external.boost.type_traits-object
        module-synthclone.external.boost.unit_test-object
        module-synthclone.external.posix-object
        module-synthclone.external.qt.core-object
        module-synthclone.external.qt.gui-object
        module-synthclone.external.qt.widgets-object
        module-synthclone.external.utf8proc-object
        module-synthclone.qt-object
        module-synthclone.qt.test-object
        module-synthclone.test-object
        module-synthclone.util-object
    )
    synthclone_configure_test(${target})
endfunction()

synthclone_make_tests()
