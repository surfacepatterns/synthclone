project(module-synthclone.app LANGUAGES CXX)

include(../../common/cmake/config.cmake)

find_package(
    Qt6
    REQUIRED
    COMPONENTS
    Concurrent
    Core
    Gui
    Qml
    Quick
    QuickControls2
)

###############################################################################
# module
###############################################################################

configure_file(
    "data/templates/cmake.cppm.tmpl"
    "${CMAKE_CURRENT_BINARY_DIR}/modules/cmake.cppm"
)
file(
    GLOB
    module-synthclone.app-cpp-sources
    src/*.cppm
    ${CMAKE_CURRENT_BINARY_DIR}/modules/*.cppm
)

file(
    GLOB module-synthclone.app-qml-sources
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    src/qml/*.qml
)

foreach(source_path ${module-synthclone.app-qml-sources})
    get_filename_component(source_name ${source_path} NAME)
    set_source_files_properties(
        ${CMAKE_CURRENT_SOURCE_DIR}/${source_path}
        PROPERTIES
        QT_RESOURCE_ALIAS ${source_name}
    )
endforeach()

set_source_files_properties(
    ${CMAKE_CURRENT_SOURCE_DIR}/src/qml/Heading.qml
    ${CMAKE_CURRENT_SOURCE_DIR}/src/qml/TabBar.qml
    ${CMAKE_CURRENT_SOURCE_DIR}/src/qml/TabButton.qml
    PROPERTIES
    QT_QML_INTERNAL_TYPE TRUE
)

function(synthclone_make_module type)
    set(target "module-synthclone.app-${type}")

    set(bootstrap_target "${target}-bootstrap")
    qt_add_library(${bootstrap_target} STATIC)
    set_property(TARGET ${bootstrap_target} PROPERTY AUTOMOC ON)

    qt_add_qml_module(
        ${bootstrap_target}
        URI Synthclone.App
        VERSION "${CMAKE_PROJECT_VERSION_MAJOR}.${CMAKE_PROJECT_VERSION_MINOR}"
        OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${type}/Synthclone/App

        DEPENDENCIES
        QtQml.Models
        QtQuick
        QtQuick.Controls.Fusion
        QtQuick.Dialogs
        QtQuick.Layouts
        Synthclone.Ui

        QML_FILES ${module-synthclone.app-qml-sources}
        NAMESPACE synthclone::app

        NO_PLUGIN
    )

    target_link_libraries(
        ${bootstrap_target}
        PRIVATE
        Qt::Core
        Qt::Gui
        Qt::Qml
        Qt::Quick
        Qt::QuickControls2
    )

    qt_add_library(${target} OBJECT)
    add_dependencies(
        ${target}
        ${bootstrap_target}
        module-synthclone.core-${type}
        module-synthclone.external.qt.concurrent-${type}
        module-synthclone.external.qt.core-${type}
        module-synthclone.external.qt.gui-${type}
        module-synthclone.external.qt.qml-${type}
        module-synthclone.external.qt.quick-${type}
        module-synthclone.external.qt.quickcontrols2-${type}
        module-synthclone.external.qt.widgets-${type}
        module-synthclone.qt-${type}
        module-synthclone.ui-${type}
        module-synthclone.util-${type}
    )
    target_include_directories(
        ${target}
        PRIVATE
        ${SYNTHCLONE_HEADER_OUTPUT_DIRECTORY}
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
        FILES ${module-synthclone.app-cpp-sources}
    )
    target_link_libraries(
        ${target}
        PRIVATE
        ${bootstrap_target}
        module-synthclone.core-${type}
        module-synthclone.external.qt.concurrent-${type}
        module-synthclone.external.qt.core-${type}
        module-synthclone.external.qt.gui-${type}
        module-synthclone.external.qt.qml-${type}
        module-synthclone.external.qt.quick-${type}
        module-synthclone.external.qt.quickcontrols2-${type}
        module-synthclone.external.qt.widgets-${type}
        module-synthclone.qt-${type}
        module-synthclone.ui-${type}
        module-synthclone.util-${type}

        # These entries are needed for the generated UI code.
        Qt6::Concurrent
        Qt6::Core
        Qt6::Gui
        Qt6::Qml
    )
    synthclone_configure_module(${target} ${type})
endfunction()

synthclone_make_module(object)

###############################################################################
# tests
###############################################################################

# XXX: Write me.
