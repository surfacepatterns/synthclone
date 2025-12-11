project(module-synthclone.ui LANGUAGES CXX)

include(../../common/cmake/config.cmake)

find_package(
    Qt6 REQUIRED
    COMPONENTS
    Core
    Qml
    Quick
    QuickControls2
)

###############################################################################
# library
###############################################################################

file(GLOB module-synthclone.ui-cpp-sources src/*.cppm)

file(
    GLOB module-synthclone.ui-qml-sources
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    src/qml/*.qml
)

foreach(source_path ${module-synthclone.ui-qml-sources})
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
set_source_files_properties(
    ${CMAKE_CURRENT_SOURCE_DIR}/src/qml/Logging.qml
    ${CMAKE_CURRENT_SOURCE_DIR}/src/qml/Style.qml
    PROPERTIES
    QT_QML_SINGLETON_TYPE TRUE
)

function(synthclone_make_module type)
    set(target "module-synthclone.ui-${type}")

    # We need to create a bootstrap target for the QML because
    # `qt_add_qml_module` doesn't support object libraries:
    #
    #     https://bugreports.qt.io/browse/QTBUG-115531
    #
    # Super frustrating.
    set(bootstrap_target "${target}-bootstrap")
    qt_add_library(${bootstrap_target} STATIC)
    set_property(TARGET ${bootstrap_target} PROPERTY AUTOMOC ON)

    # XXX: File an upstream bug.
    #
    # There is a bug in `qmlcachegen` that creates incorrect identifiers if the
    # target name contains a '.'.  This appears to be due to the fact that the
    # resource name passed to `qmlcachegen` by `qt_add_qml_module` is formed as
    # `qmlcache_${target_name}`:
    #
    # https://github.com/qt/qtdeclarative/\
    # blob/b00d71760be5695bb97afc991cf1e21ca9730fb1/src/qml/\
    # Qt6QmlMacros.cmake#L1807
    #
    # ... which is passed to `qmlcachegen` via the `--resource-name` parameter:
    #
    # https://github.com/qt/qtdeclarative/blob/\
    # b00d71760be5695bb97afc991cf1e21ca9730fb1/src/qml/Qt6QmlMacros.cmake#L1846
    #
    # ... which, in `main()`, is passed to `qmlJSGenerateLoader()`:
    #
    # https://github.com/qt/qtdeclarative/blob/dev/tools/qmlcachegen/\
    # qmlcachegen.cpp#L194
    #
    # ... which assumes the string is a file name and passes it to
    # `qtResourcesNameForFile()`:
    #
    # https://github.com/qt/qtdeclarative/blob/dev/src/qmlcompiler/\
    # qqmljsloadergenerator.cpp#L170
    #
    # ... which truncates any "file extension" in the name:
    #
    # https://github.com/qt/qtdeclarative/blob/dev/src/qmlcompiler/\
    # qqmljsloadergenerator.cpp#L85
    #
    # ... resulting in an incorrect identifier.
    #
    # For whatever reason, the bug doesn't currently prevent this target from
    # working.
    qt_add_qml_module(
        ${bootstrap_target}
        URI Synthclone.Ui
        VERSION "${CMAKE_PROJECT_VERSION_MAJOR}.${CMAKE_PROJECT_VERSION_MINOR}"
        OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${type}/Synthclone/Ui

        DEPENDENCIES
        QtQml.Models
        QtQuick
        QtQuick.Controls.Fusion
        QtQuick.Dialogs
        QtQuick.Layouts

        QML_FILES ${module-synthclone.ui-qml-sources}
        NAMESPACE synthclone::ui

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

    # Now, we want to take the objects that make up the bootstrap library and
    # package them in an object library with the expected name.
    add_library(${target} OBJECT $<TARGET_OBJECTS:${bootstrap_target}>)
    add_dependencies(
        ${target}
        ${bootstrap_target}
        module-synthclone.external.boost.urls-${type}
        module-synthclone.external.qt.concurrent-${type}
        module-synthclone.external.qt.core-${type}
        module-synthclone.external.qt.gui-${type}
        module-synthclone.external.qt.quick-${type}
        module-synthclone.qt-${type}
        module-synthclone.util-${type}
    )
    target_sources(
        ${target}
        PUBLIC
        FILE_SET CXX_MODULES
        TYPE CXX_MODULES
        FILES ${module-synthclone.ui-cpp-sources}
    )
    target_link_libraries(
        ${target}
        PRIVATE
        ${bootstrap_target}
        module-synthclone.external.boost.callable_traits-${type}
        module-synthclone.external.boost.core-${type}
        module-synthclone.external.boost.mp11-${type}
        module-synthclone.external.boost.scope-${type}
        module-synthclone.external.boost.urls-${type}
        module-synthclone.external.posix-${type}
        module-synthclone.external.qt.concurrent-${type}
        module-synthclone.external.qt.core-${type}
        module-synthclone.external.qt.gui-${type}
        module-synthclone.external.qt.qml-${type}
        module-synthclone.external.qt.quick-${type}
        module-synthclone.external.utf8proc-${type}
        module-synthclone.qt-${type}
        module-synthclone.util-${type}

        Qt::Core
        Qt::Gui
        Qt::Qml
        Qt::Quick
        Qt::QuickControls2
    )
    synthclone_configure_module(${target} ${type})
endfunction()

synthclone_make_module(object)
synthclone_make_module(shared)
