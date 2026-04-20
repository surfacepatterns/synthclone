project(module-synthclone.util LANGUAGES CXX)

include(../../common/cmake/config.cmake)

###############################################################################
# module
###############################################################################

configure_file(
    "data/templates/config.h.tmpl"
    "${SYNTHCLONE_HEADER_OUTPUT_DIRECTORY}/synthclone/config.h"
)

file(GLOB module-synthclone.util-sources src/*.cppm)

function(synthclone_make_module type)
    set(target "module-synthclone.util-${type}")
    add_library(${target} OBJECT)
    add_dependencies(
        ${target}
        module-synthclone.external.boost.io-${type}
        module-synthclone.external.boost.mp11-${type}
        module-synthclone.external.boost.numeric-${type}
        module-synthclone.external.boost.scope-${type}
        module-synthclone.external.posix-${type}
        module-synthclone.external.utf8proc-${type}
    )
    target_include_directories(
        ${target}
        PUBLIC
        ${SYNTHCLONE_HEADER_OUTPUT_DIRECTORY}
    )
    target_sources(
        ${target}
        PUBLIC
        FILE_SET CXX_MODULES
        TYPE CXX_MODULES
        FILES
        ${module-synthclone.util-sources}
    )
    target_link_libraries(
        ${target}
        PRIVATE
        module-synthclone.external.boost.io-${type}
        module-synthclone.external.boost.mp11-${type}
        module-synthclone.external.boost.numeric-${type}
        module-synthclone.external.boost.scope-${type}
        module-synthclone.external.posix-${type}
        module-synthclone.external.utf8proc-${type}
    )
    synthclone_configure_module(${target} ${type})
endfunction()

synthclone_make_module(object)
synthclone_make_module(shared)

###############################################################################
# tests
###############################################################################

file(GLOB module-synthclone.util-tests tests/*.cpp)

function(synthclone_make_tests)
    set(target "run-synthclone-util-tests")
    add_executable(${target})
    add_dependencies(
        ${target}
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
        PRIVATE
        ${module-synthclone.util-tests}
    )
    target_link_libraries(
        ${target}
        PRIVATE
        module-synthclone.external.boost.core-object
        module-synthclone.external.boost.io-object
        module-synthclone.external.boost.mp11-object
        module-synthclone.external.boost.numeric-object
        module-synthclone.external.boost.scope-object
        module-synthclone.external.boost.type_traits-object
        module-synthclone.external.boost.unit_test-object
        module-synthclone.external.posix-object
        module-synthclone.external.utf8proc-object
        module-synthclone.test-object
        module-synthclone.util-object
    )
    synthclone_configure_test(${target})
endfunction()

synthclone_make_tests()
