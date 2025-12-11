project(module-synthclone.fx LANGUAGES CXX)

include(../../common/cmake/config.cmake)

find_package(Qt6 REQUIRED COMPONENTS Core)

###############################################################################
# library
###############################################################################

file(GLOB module-synthclone.fx-sources src/*.cppm)

function(synthclone_make_module type)
    set(target "module-synthclone.fx-${type}")
    qt_add_library(${target} OBJECT)
    add_dependencies(
        ${target}
        module-synthclone.core-${type}
        module-synthclone.external.boost.interprocess-${type}
        module-synthclone.external.qt.qml-${type}
        module-synthclone.plugin-${type}
        module-synthclone.qt-${type}
        module-synthclone.util-${type}
    )
    target_sources(
        ${target}
        PUBLIC
        FILE_SET CXX_MODULES
        TYPE CXX_MODULES
        FILES ${module-synthclone.fx-sources}
    )
    target_link_libraries(
        ${target}
        PRIVATE
        module-synthclone.core-${type}
        module-synthclone.external.boost.interprocess-${type}
        module-synthclone.external.qt.qml-${type}
        module-synthclone.plugin-${type}
        module-synthclone.qt-${type}
        module-synthclone.util-${type}

        # These links are needed for includes associated with the generated UI
        # code.
        Qt6::Core
    )
    synthclone_configure_module(${target} ${type})
endfunction()

synthclone_make_module(object)
synthclone_make_module(shared)

###############################################################################
# tests
###############################################################################

file(GLOB module-synthclone.fx-tests tests/*.cpp)

function(synthclone_make_tests)
    set(target "run-synthclone-fx-tests")
    add_executable(${target})
    add_dependencies(
        ${target}
        module-synthclone.core-object
        module-synthclone.external.boost.unit_test-object
        module-synthclone.fx-object
        module-synthclone.test-object
        module-synthclone.test.plugin-object
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
        ${module-synthclone.fx-tests}
    )
    target_link_libraries(
        ${target}
        PRIVATE
        module-synthclone.core-object
        module-synthclone.external.boost.callable_traits-object
        module-synthclone.external.boost.container-object
        module-synthclone.external.boost.core-object
        module-synthclone.external.boost.interprocess-object
        module-synthclone.external.boost.mp11-object
        module-synthclone.external.boost.scope-object
        module-synthclone.external.boost.type_traits-object
        module-synthclone.external.boost.unit_test-object
        module-synthclone.external.boost.urls-object
        module-synthclone.external.posix-object
        module-synthclone.external.qt.core-object
        module-synthclone.external.qt.gui-object
        module-synthclone.external.qt.qml-object
        module-synthclone.external.samplerate-object
        module-synthclone.external.sndfile-object
        module-synthclone.external.utf8proc-object
        module-synthclone.fx-object
        module-synthclone.plugin-object
        module-synthclone.qt-object
        module-synthclone.test-object
        module-synthclone.test.plugin-object
        module-synthclone.util-object
    )
    synthclone_configure_test(${target})
endfunction()

synthclone_make_tests()
