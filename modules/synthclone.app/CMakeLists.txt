project(module-synthclone.app LANGUAGES CXX)

include(../../common/cmake/config.cmake)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

###############################################################################
# module
###############################################################################

configure_file(
    "data/templates/cmake.cppm.tmpl"
    "${CMAKE_CURRENT_BINARY_DIR}/modules/cmake.cppm"
)
file(
    GLOB
    module-synthclone.app-sources
    src/*.cppm
    ${CMAKE_CURRENT_BINARY_DIR}/modules/*.cppm
)

function(synthclone_make_module type)
    set(target "module-synthclone.app-${type}")
    qt_add_library(${target} OBJECT)
    add_dependencies(
        ${target}
        module-synthclone.core-${type}
        module-synthclone.external.qt.core-${type}
        module-synthclone.external.qt.gui-${type}
        module-synthclone.external.qt.widgets-${type}
        module-synthclone.qt-${type}
        module-synthclone.util-${type}
    )
    qt_add_ui(
        ${target}
        SOURCES
        data/ui/about_dialog.ui
    )
    qt_add_resources(
        ${target}
        "synthclone.app"
        PREFIX "/synthclone.app"
        BASE "../.."
        FILES
        "../../LICENSE.md"
        "../../README.md"
    )
    target_sources(
        ${target}
        PUBLIC
        FILE_SET CXX_MODULES
        TYPE CXX_MODULES
        BASE_DIRS
        src
        ${CMAKE_CURRENT_BINARY_DIR}/modules
        FILES ${module-synthclone.app-sources}
    )
    target_link_libraries(
        ${target}
        PRIVATE
        module-synthclone.core-${type}
        module-synthclone.external.qt.core-${type}
        module-synthclone.external.qt.gui-${type}
        module-synthclone.external.qt.widgets-${type}
        module-synthclone.qt-${type}
        module-synthclone.util-${type}

        # These entries are needed for the generated UI code.
        Qt6::Core
        Qt6::Widgets
    )
    synthclone_configure_module(${target} ${type})
endfunction()

synthclone_make_module(object)

###############################################################################
# tests
###############################################################################

# XXX: Write me.
