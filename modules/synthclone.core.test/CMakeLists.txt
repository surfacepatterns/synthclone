project(module-synthclone.core.test LANGUAGES CXX)

include(../../common/cmake/config.cmake)

file(GLOB module-synthclone.core.test-sources src/*.cppm)

function(synthclone_make_module type)
    set(target "module-synthclone.core.test-${type}")
    add_library(${target} OBJECT)
    add_dependencies(
        ${target}
        module-synthclone.core-${type}
        module-synthclone.external.kissfft-${type}
        module-synthclone.test-${type}
        module-synthclone.util-${type}
    )
    target_sources(
        ${target}
        PUBLIC
        FILE_SET CXX_MODULES
        TYPE CXX_MODULES
        FILES ${module-synthclone.core.test-sources}
    )
    target_link_libraries(
        ${target}
        PRIVATE
        module-synthclone.core-${type}
        module-synthclone.external.kissfft-${type}
        module-synthclone.test-${type}
        module-synthclone.util-${type}
    )
    synthclone_configure_module(${target} ${type})
endfunction()

synthclone_make_module(object)
