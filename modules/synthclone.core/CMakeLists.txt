project(synthclone.core LANGUAGES CXX)

include(../../common/cmake/config.cmake)

###############################################################################
# module
###############################################################################

configure_file(
    "data/templates/project.cppm.tmpl"
    "${CMAKE_CURRENT_BINARY_DIR}/modules/project.cppm"
)
file(
    GLOB
    module-synthclone.core-sources
    src/*.cppm
    ${CMAKE_CURRENT_BINARY_DIR}/modules/*.cppm
)

function(synthclone_make_module type)
    set(target "module-synthclone.core-${type}")
    add_library(${target} OBJECT)
    add_dependencies(
        ${target}
        module-synthclone.external.boost.container-${type}
        module-synthclone.external.boost.interprocess-${type}
        module-synthclone.external.boost.mp11-${type}
        module-synthclone.external.boost.scope-${type}
        module-synthclone.external.boost.urls-${type}
        module-synthclone.external.samplerate-${type}
        module-synthclone.external.sndfile-${type}
        module-synthclone.util-${type}
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
        BASE_DIRS
        src
        ${CMAKE_CURRENT_BINARY_DIR}/modules
        FILES
        ${module-synthclone.core-sources}
    )
    target_link_libraries(
        ${target}
        PRIVATE
        module-synthclone.external.boost.container-${type}
        module-synthclone.external.boost.interprocess-${type}
        module-synthclone.external.boost.mp11-${type}
        module-synthclone.external.boost.scope-${type}
        module-synthclone.external.boost.urls-${type}
        module-synthclone.external.samplerate-${type}
        module-synthclone.external.sndfile-${type}
        module-synthclone.util-${type}
    )
    synthclone_configure_module(${target} ${type})
endfunction()

synthclone_make_module(object)
synthclone_make_module(shared)

###############################################################################
# tests
###############################################################################

file(GLOB module-synthclone.core-tests tests/*.cpp)

function(synthclone_make_tests)
    set(target "run-synthclone-core-tests")
    add_executable(${target})
    add_dependencies(
        ${target}
        module-synthclone.core-object
        module-synthclone.core.test-object
        module-synthclone.external.boost.container-object
        module-synthclone.external.boost.interprocess-object
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
        ${module-synthclone.core-tests}
    )
    target_link_libraries(
        ${target}
        PRIVATE
        module-synthclone.core-object
        module-synthclone.core.test-object
        module-synthclone.external.boost.container-object
        module-synthclone.external.boost.core-object
        module-synthclone.external.boost.interprocess-object
        module-synthclone.external.boost.mp11-object
        module-synthclone.external.boost.scope-object
        module-synthclone.external.boost.type_traits-object
        module-synthclone.external.boost.urls-object
        module-synthclone.external.kissfft-object
        module-synthclone.external.posix-object
        module-synthclone.external.samplerate-object
        module-synthclone.external.sndfile-object
        module-synthclone.external.utf8proc-object
        module-synthclone.test-object
        module-synthclone.util-object
    )
    synthclone_configure_test(${target})
endfunction()

synthclone_make_tests()
