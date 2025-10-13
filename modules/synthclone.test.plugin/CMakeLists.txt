project(module-synthclone.test.plugin LANGUAGES CXX)

include(../../common/cmake/config.cmake)

file(GLOB module-synthclone.test.plugin-sources src/*.cppm)

function(synthclone_make_module type)
    set(target "module-synthclone.test.plugin-${type}")
    add_library(${target} OBJECT)
    add_dependencies(
        ${target}
        module-synthclone.core-${type}
        module-synthclone.test-${type}
    )
    target_sources(
        ${target}
        PUBLIC
        FILE_SET CXX_MODULES
        TYPE CXX_MODULES
        FILES ${module-synthclone.test.plugin-sources}
    )
    target_link_libraries(
        ${target}
        PRIVATE
        module-synthclone.core-${type}
        module-synthclone.test-${type}
    )
    synthclone_configure_module(${target} ${type})
endfunction()

synthclone_make_module(object)
