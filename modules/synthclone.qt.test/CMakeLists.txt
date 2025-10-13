project(module-synthclone.qt.test LANGUAGES CXX)

include(../../common/cmake/config.cmake)

find_package(Qt6 REQUIRED COMPONENTS Core)

function(synthclone_make_module type)
    # You might ask yourself why this "library" contains a single resource and
    # no real code.  The answer is that `qt_add_executable`, which is required
    # to add resources to an executable using `qt_add_resources`, *breaks*
    # module imports, meaning we can't add the resource to
    # `run-synthclone-qt-tests`.
    set(target "module-synthclone.qt.test-${type}")
    qt_add_library(${target} OBJECT)
    add_dependencies(${target} module-synthclone.external.qt.core-${type})
    qt_add_resources(
        ${target} "synthclone.qt.test"
        PREFIX "/synthclone.qt.test"
        BASE "data"
        FILES
        "data/qresource/foo"
    )
    target_link_libraries(
        ${target}
        PRIVATE
        module-synthclone.external.qt.core-${type}
    )
    synthclone_configure_module(${target} ${type})
endfunction()

synthclone_make_module(object)
