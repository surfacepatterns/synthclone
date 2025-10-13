project(module-synthclone.external.sndfile LANGUAGES CXX)

include(../../common/cmake/config.cmake)

file(GLOB module-synthclone.external.sndfile-sources src/*.cppm)

function(synthclone_make_module type)
    set(target "module-synthclone.external.sndfile-${type}")
    add_library(${target} OBJECT)
    target_sources(
        ${target}
        PUBLIC
        FILE_SET CXX_MODULES
        TYPE CXX_MODULES
        FILES ${module-synthclone.external.sndfile-sources}
    )
    target_link_libraries(${target} PRIVATE sndfile)
    synthclone_configure_module(${target} ${type})
endfunction()

synthclone_make_module(object)
synthclone_make_module(shared)
