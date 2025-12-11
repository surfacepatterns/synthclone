project(module-synthclone.external.qt.qml LANGUAGES CXX)

include(../../common/cmake/config.cmake)

find_package(Qt6 REQUIRED COMPONENTS Core Qml)

file(GLOB module-synthclone.external.qt.qml-sources src/*.cppm)

function(synthclone_make_module type)
    set(target "module-synthclone.external.qt.qml-${type}")
    add_library(${target} OBJECT)
    target_sources(
        ${target}
        PUBLIC
        FILE_SET CXX_MODULES
        TYPE CXX_MODULES
        FILES ${module-synthclone.external.qt.qml-sources}
    )
    target_link_libraries(${target} PRIVATE Qt6::Qml)
    synthclone_configure_module(${target} ${type})
endfunction()

synthclone_make_module(object)
synthclone_make_module(shared)
