project(module-synthclone.external.qt.quickcontrols2 LANGUAGES CXX)

include(../../common/cmake/config.cmake)

find_package(Qt6 REQUIRED COMPONENTS Core QuickControls2)

file(GLOB module-synthclone.external.qt.quickcontrols2-sources src/*.cppm)

function(synthclone_make_module type)
    set(target "module-synthclone.external.qt.quickcontrols2-${type}")
    add_library(${target} OBJECT)
    target_sources(
        ${target}
        PUBLIC
        FILE_SET CXX_MODULES
        TYPE CXX_MODULES
        FILES ${module-synthclone.external.qt.quickcontrols2-sources}
    )
    target_link_libraries(${target} PRIVATE Qt6::QuickControls2)
    synthclone_configure_module(${target} ${type})
endfunction()

synthclone_make_module(object)
synthclone_make_module(shared)
