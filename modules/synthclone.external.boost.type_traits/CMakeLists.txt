project(module-synthclone.external.boost.type_traits LANGUAGES CXX)

include(../../common/cmake/config.cmake)

file(GLOB module-synthclone.external.boost.type_traits-sources src/*.cppm)

function(synthclone_make_module type)
    set(target "module-synthclone.external.boost.type_traits-${type}")
    add_library(${target} OBJECT)
    target_sources(
        ${target}
        PUBLIC
        FILE_SET CXX_MODULES
        TYPE CXX_MODULES
        FILES ${module-synthclone.external.boost.type_traits-sources}
    )
    synthclone_configure_module(${target} ${type})
endfunction()

synthclone_make_module(object)
synthclone_make_module(shared)
