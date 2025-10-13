project(module-synthclone.plugin LANGUAGES CXX)

include(../../common/cmake/config.cmake)

###############################################################################
# module
###############################################################################

file(GLOB module-synthclone.plugin-sources src/*.cppm)

function(synthclone_make_module type)
    set(target "module-synthclone.plugin-${type}")
    add_library(${target} OBJECT)
    add_dependencies(${target} module-synthclone.core-${type})
    target_sources(
        ${target}
        PUBLIC
        FILE_SET CXX_MODULES
        TYPE CXX_MODULES
        FILES
        ${module-synthclone.plugin-sources}
    )
    target_link_libraries(
        ${target}
        PRIVATE
        module-synthclone.core-${type}
    )
    synthclone_configure_module(${target} ${type})
endfunction()

synthclone_make_module(object)
synthclone_make_module(shared)

###############################################################################
# tests
###############################################################################

file(GLOB module-synthclone.plugin-tests tests/*.cpp)

function(synthclone_make_tests)
    set(target "run-synthclone-plugin-tests")
    add_executable(${target})
    add_dependencies(
        ${target}
        module-synthclone.core-object
        module-synthclone.plugin-object
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
        ${module-synthclone.plugin-tests}
    )
    target_link_libraries(
        ${target}
        PRIVATE
        module-synthclone.core-object
        module-synthclone.external.boost.container-object
        module-synthclone.external.boost.core-object
        module-synthclone.external.boost.interprocess-object
        module-synthclone.external.boost.mp11-object
        module-synthclone.external.boost.scope-object
        module-synthclone.external.boost.type_traits-object
        module-synthclone.external.boost.urls-object
        module-synthclone.external.posix-object
        module-synthclone.external.samplerate-object
        module-synthclone.external.sndfile-object
        module-synthclone.external.utf8proc-object
        module-synthclone.plugin-object
        module-synthclone.test-object
        module-synthclone.util-object
    )
    synthclone_configure_test(${target})
endfunction()

synthclone_make_tests()
