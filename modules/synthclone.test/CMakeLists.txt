project(module-synthclone.test LANGUAGES CXX)

include(../../common/cmake/config.cmake)

file(GLOB module-synthclone.test-sources src/*.cppm)

function(synthclone_make_module type)
    set(target "module-synthclone.test-${type}")
    add_library(${target} OBJECT)
    add_dependencies(
        ${target}
        module-synthclone.external.boost.core-${type}
        module-synthclone.external.boost.type_traits-${type}
        module-synthclone.util-${type}
    )
    target_sources(
        ${target}
        PUBLIC
        FILE_SET CXX_MODULES
        TYPE CXX_MODULES
        FILES ${module-synthclone.test-sources}
    )
    target_link_libraries(
        ${target}
        PRIVATE
        module-synthclone.external.boost.core-${type}
        module-synthclone.external.boost.type_traits-${type}
        module-synthclone.util-${type}
    )
    synthclone_configure_module(${target} ${type})
endfunction()

synthclone_make_module(object)
