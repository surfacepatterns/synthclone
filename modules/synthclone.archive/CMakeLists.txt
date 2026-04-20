project(module-synthclone.archive LANGUAGES CXX)

include(../../common/cmake/config.cmake)

###############################################################################
# module
###############################################################################

file(GLOB module-synthclone.archive-sources src/*.cppm)

function(synthclone_make_module type)
    set(target "module-synthclone.archive-${type}")
    add_library(${target} OBJECT)
    add_dependencies(
        ${target}
        module-synthclone.external.archive-${type}
        module-synthclone.external.boost.interprocess-${type}
        module-synthclone.external.boost.io-${type}
        module-synthclone.external.posix-${type}
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
        FILES
        ${module-synthclone.archive-sources}
    )
    target_link_libraries(
        ${target}
        PRIVATE
        module-synthclone.external.archive-${type}
        module-synthclone.external.boost.interprocess-${type}
        module-synthclone.external.boost.io-${type}
        module-synthclone.external.posix-${type}
        module-synthclone.util-${type}
    )
    synthclone_configure_module(${target} ${type})
endfunction()

synthclone_make_module(object)
synthclone_make_module(shared)

###############################################################################
# tests
###############################################################################

file(GLOB module-synthclone.archive-tests tests/*.cpp)

function(synthclone_make_tests)
    set(target "run-synthclone-archive-tests")
    add_executable(${target})
    add_dependencies(
        ${target}
        module-synthclone.archive-object
        module-synthclone.test-object
    )
    add_test(
        NAME ${target}
        COMMAND ${target}
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )
    target_sources(
        ${target}
        PRIVATE
        ${module-synthclone.archive-tests}
    )
    target_link_libraries(
        ${target}
        PRIVATE
        module-synthclone.archive-object
        module-synthclone.external.archive-object
        module-synthclone.external.boost.interprocess-object
        module-synthclone.external.boost.io-object
        module-synthclone.external.boost.unit_test-object
        module-synthclone.external.posix-object
        module-synthclone.test-object
        module-synthclone.util-object
    )
    synthclone_configure_test(${target})
endfunction()

synthclone_make_tests()
