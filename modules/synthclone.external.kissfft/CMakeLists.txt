project(module-synthclone.external.kissfft LANGUAGES CXX)

include(../../common/cmake/config.cmake)

file(GLOB module-synthclone.external.kissfft-sources src/*.cppm)

function(synthclone_make_module type)
    set(target "module-synthclone.external.kissfft-${type}")
    add_library(${target} OBJECT)
    target_sources(
        ${target}
        PUBLIC
        FILE_SET CXX_MODULES
        TYPE CXX_MODULES
        FILES ${module-synthclone.external.kissfft-sources}
    )
    target_link_libraries(${target} PRIVATE kissfft-float)
    synthclone_configure_module(${target} ${type})
endfunction()

synthclone_make_module(object)
