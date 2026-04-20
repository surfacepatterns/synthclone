project(module-synthclone.external.boost.json LANGUAGES CXX)

include(../../common/cmake/config.cmake)

file(GLOB module-synthclone.external.boost.json-sources src/*.cppm)

function(synthclone_make_module type)
    set(target "module-synthclone.external.boost.json-${type}")
    add_library(${target} OBJECT)
    target_sources(
        ${target}
        PUBLIC
        FILE_SET CXX_MODULES
        TYPE CXX_MODULES
        FILES ${module-synthclone.external.boost.json-sources}
    )
    target_link_libraries(${target} boost_json)
    synthclone_configure_module(${target} ${type})
endfunction()

synthclone_make_module(object)
synthclone_make_module(shared)
