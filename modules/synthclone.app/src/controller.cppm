export module synthclone.app:controller;

import synthclone.external.qt.gui;
import synthclone.external.qt.qml;
import synthclone.external.qt.quick;
import synthclone.external.qt.quickcontrols2;
import synthclone.qt;
import synthclone.ui;

import :about;

///////////////////////////////////////////////////////////////////////////////
// synthclone::run()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    int
    run(int argc, char** argv)
    {
        ::QGuiApplication application(argc, argv);

        // auto iter = ::QDirIterator(":", ::QDirIterator::Subdirectories);
        // while (iter.hasNext()) {
        //     std::println("resource: {0}", iter.next().toStdString());
        // }

        // XXX: Perhaps this belongs in `synthclone.ui`.
        ::QQuickStyle::setStyle("Fusion");

        // Setup the QML engine so that QML in `synthclone.ui` functions the
        // way it's expected to function.
        //
        // XXX: Perhaps this belongs in `synthclone.ui`.
        ::QQmlEngine engine;

        auto theme_icon_provider =
            std::make_unique<synthclone::theme_icon_provider>();
        engine.addImageProvider("themeicon", theme_icon_provider.get());
        theme_icon_provider.release();

        // Main window.
        auto main_window_component = load_qml_component(
            &engine, "Synthclone.App", "ApplicationWindow");
        auto main_window = create(*main_window_component);

        // About view.
        about_view about_view(
            find_child<::QObject>(main_window, "aboutDialog"));

        // Connections
        qobject_connection_guard about_synthclone_connection_guard(
            find_child<::QObject>(main_window, "aboutSynthcloneMenuItem"),
            "triggered()",
            [&application, &about_view]() {
                for (const auto message: about_view.run()) {
                    application.processEvents(::QEventLoop::WaitForMoreEvents);
                }
            });
        qobject_connection_guard quit_connection_guard(
            find_child<::QObject>(main_window, "quitMenuItem"),
            "triggered()", [&application]() { application.quit(); });

        // And go!
        set_property_value(main_window, "visible", true);
        return application.exec();
    }

}
