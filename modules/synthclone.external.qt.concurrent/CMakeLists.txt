project(module-synthclone.external.qt.concurrent LANGUAGES CXX)

include(../../common/cmake/config.cmake)

find_package(Qt6 REQUIRED COMPONENTS Concurrent)

file(GLOB module-synthclone.external.qt.concurrent-sources src/*.cppm)

function(synthclone_make_module type)
    set(target "module-synthclone.external.qt.concurrent-${type}")
    add_library(${target} OBJECT)
    target_sources(
        ${target}
        PUBLIC
        FILE_SET CXX_MODULES
        TYPE CXX_MODULES
        FILES ${module-synthclone.external.qt.concurrent-sources}
    )
    target_link_libraries(${target} PRIVATE Qt6::Concurrent)
    synthclone_configure_module(${target} ${type})
endfunction()

synthclone_make_module(object)
synthclone_make_module(shared)
