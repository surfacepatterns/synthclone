project(app-synthclone LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Qml)

include(../../common/cmake/config.cmake)

file(GLOB app-synthclone-sources src/*.cpp)

function(synthclone_make_app)
    set(target "synthclone")
    add_executable(${target})
    add_dependencies(${target} module-synthclone.app-object)
    target_sources(${target} PUBLIC ${app-synthclone-sources})
    target_link_libraries(
        ${target}
        PRIVATE
        module-synthclone.app-object
        module-synthclone.core-object
        module-synthclone.external.boost.callable_traits-object
        module-synthclone.external.boost.container-object
        module-synthclone.external.boost.core-object
        module-synthclone.external.boost.interprocess-object
        module-synthclone.external.boost.mp11-object
        module-synthclone.external.boost.scope-object
        module-synthclone.external.boost.urls-object
        module-synthclone.external.posix-object
        module-synthclone.external.qt.concurrent-object
        module-synthclone.external.qt.core-object
        module-synthclone.external.qt.gui-object
        module-synthclone.external.qt.qml-object
        module-synthclone.external.qt.quick-object
        module-synthclone.external.qt.quickcontrols2-object
        module-synthclone.external.qt.widgets-object
        module-synthclone.external.samplerate-object
        module-synthclone.external.sndfile-object
        module-synthclone.external.utf8proc-object
        module-synthclone.qt-object
        module-synthclone.ui-object
        module-synthclone.util-object

        stdc++exp
    )
    synthclone_configure_app(${target})
endfunction()

synthclone_make_app()
