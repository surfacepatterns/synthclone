export module synthclone.app:about;

import std;

import synthclone.core;
import synthclone.external.qt.core;
import synthclone.qt;
import synthclone.util;

import :cmake;
import :message;

///////////////////////////////////////////////////////////////////////////////
// synthclone::about_view
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    qobject_ptr<::QObject>
    populate_about_dialog(qobject_ptr<::QObject> item)
    {
        set_property_value(
            item, "title",
            ::QString::fromStdString(
                std::format(
                    "About {0} {1}.{2}.{3}", cmake_project_name,
                    project_version_major, project_version_minor,
                    project_version_patch)));

        // Application
        set_property_value(
            find_child<::QObject>(item, "applicationDescriptionLabel"), "text",
            ::QString(load_qresource_bytes(":/synthclone.app/README.md")));

        // Build
        set_property_value(
            find_child<::QObject>(item, "buildSystemLabel"), "text",
            ::QString::fromStdString(
                std::format(
                    "{0} {1} ({2})", cmake_system_name, cmake_system_version,
                    cmake_system_processor)));
        set_property_value(
            find_child<::QObject>(item, "buildLibraryArchitectureLabel"),
            "text", ::QString::fromLocal8Bit(cmake_cxx_library_architecture));
        set_property_value(
            find_child<::QObject>(item, "buildCMakeLabel"), "text",
            ::QString::fromLocal8Bit(cmake_version));
        set_property_value(
            find_child<::QObject>(item, "buildGeneratorLabel"), "text",
            ::QString::fromLocal8Bit(cmake_generator));
        set_property_value(
            find_child<::QObject>(item, "buildCompilerLabel"), "text",
            ::QString::fromStdString(
                std::format(
                    "{0} {1} ({2})", cmake_cxx_compiler_id,
                    cmake_cxx_compiler_version,
                    cmake_cxx_compiler_architecture_id)));
        set_property_value(
            find_child<::QObject>(item, "buildTypeLabel"), "text",
            ::QString::fromLocal8Bit(cmake_build_type));
        set_property_value(
            find_child<::QObject>(item, "buildCppFlagsLabel"), "text",
            ::QString::fromLocal8Bit(cmake_cxx_flags));
        set_property_value(
            find_child<::QObject>(item, "buildCppStandardLabel"), "text",
            ::QString::fromLocal8Bit(cmake_cxx_standard));
        set_property_value(
            find_child<::QObject>(item, "buildInstallPrefixLabel"), "text",
            ::QString::fromLocal8Bit(cmake_install_prefix));

        // License
        set_property_value(
            find_child<::QObject>(item, "licenseDescriptionLabel"), "text",
            ::QString(load_qresource_bytes(":/synthclone.app/LICENSE.md")));

        return item;
    }

    export
    class about_view final: private nonmovable {

    public:

        explicit
        about_view(qobject_ptr<::QObject> item):
            item_(populate_about_dialog(item))
        {
            // empty
        }

        std::generator<event_wait_message>
        run()
        {
            bool closed = false;

            qobject_connection_guard connection_guard(
                item_, "closed()", [&closed]() noexcept { closed = true; });
            invoke_method(item_, "open()");

            while (! closed) {
                co_yield event_wait_message();
            }
        }

    private:

        qobject_ptr<::QObject> item_;

    };

}
