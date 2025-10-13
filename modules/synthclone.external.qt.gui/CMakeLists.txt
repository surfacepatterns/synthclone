project(module-synthclone.external.qt.gui LANGUAGES CXX)

include(../../common/cmake/config.cmake)

find_package(Qt6 REQUIRED COMPONENTS Gui)

file(GLOB module-synthclone.external.qt.gui-sources src/*.cppm)

function(synthclone_make_module type)
    set(target "module-synthclone.external.qt.gui-${type}")
    add_library(${target} OBJECT)
    target_sources(
        ${target}
        PUBLIC
        FILE_SET CXX_MODULES
        TYPE CXX_MODULES
        FILES ${module-synthclone.external.qt.gui-sources}
    )
    target_link_libraries(${target} PRIVATE Qt6::Gui)
    synthclone_configure_module(${target} ${type})
endfunction()

synthclone_make_module(object)
synthclone_make_module(shared)
