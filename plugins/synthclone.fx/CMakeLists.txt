project(plugin-synthclone.fx LANGUAGES CXX)

include(../../common/cmake/config.cmake)

file(GLOB plugin-synthclone.fx-sources src/*.cpp)

function(synthclone_make_plugin)
    set(target "synthclone.fx")
    add_library(${target} MODULE)
    add_dependencies(${target} module-synthclone.fx-shared)
    target_sources(
        ${target}
        PRIVATE
        ${plugin-synthclone.fx-sources}
    )
    target_link_libraries(
        ${target}
        PRIVATE
        module-synthclone.core-shared
        module-synthclone.external.boost.callable_traits-shared
        module-synthclone.external.boost.container-shared
        module-synthclone.external.boost.interprocess-shared
        module-synthclone.external.boost.json-shared
        module-synthclone.external.boost.mp11-shared
        module-synthclone.external.boost.scope-shared
        module-synthclone.external.boost.urls-shared
        module-synthclone.external.posix-shared
        module-synthclone.external.qt.core-shared
        module-synthclone.external.qt.gui-shared
        module-synthclone.external.qt.qml-shared
        module-synthclone.external.qt.widgets-shared
        module-synthclone.external.samplerate-shared
        module-synthclone.external.sndfile-shared
        module-synthclone.external.utf8proc-shared
        module-synthclone.fx-shared
        module-synthclone.qt-shared
        module-synthclone.util-shared
    )
    synthclone_configure_plugin(${target})
endfunction()

synthclone_make_plugin()
