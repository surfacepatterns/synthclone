module;

#include "ui_about_dialog.h"

export module synthclone.app:about;

import std;

import synthclone.core;
import synthclone.external.qt;
import synthclone.qt;
import synthclone.util;

import :cmake;
import :html;
import :message;

///////////////////////////////////////////////////////////////////////////////
// synthclone::about_view
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    ::QString
    compose_application_markup()
    {
        ::QString result;
        ::QTextStream stream(&result);

        write_html_start(stream);

        write_html_section_start(
            stream, html_section_heading::h3,
            std::format(
                "{0} {1}.{2}.{3}", cmake_project_name, project_version_major,
                project_version_minor, project_version_patch));
        write_html_markdown(
            stream,
            ::QString(load_qresource_bytes(":/synthclone.app/README.md")));
        write_html_section_end(stream);

        write_html_end(stream);

        return result;
    }

    ::QString
    compose_build_markup()
    {
        ::QString result;
        ::QTextStream stream(&result);

        write_html_start(stream);

        write_html_section_start(
            stream, html_section_heading::h3, "Environment");

        write_html_table_start(stream);

        write_html_table_row_start(stream);
        write_html_table_cell(stream, "System:");
        write_html_table_cell(
            stream,
            std::format(
                "{0} {1} ({2})", cmake_system_name, cmake_system_version,
                cmake_system_processor));
        write_html_table_row_end(stream);

        write_html_table_row_start(stream);
        write_html_table_cell(stream, "Library Architecture:");
        write_html_table_cell(stream, cmake_cxx_library_architecture);
        write_html_table_row_end(stream);

        write_html_table_end(stream);

        write_html_section_end(stream);

        write_html_section_start(stream, html_section_heading::h3, "Tools");

        write_html_table_start(stream);

        write_html_table_row_start(stream);
        write_html_table_cell(stream, "CMake:");
        write_html_table_cell(stream, cmake_version);
        write_html_table_row_end(stream);

        write_html_table_row_start(stream);
        write_html_table_cell(stream, "Generator:");
        write_html_table_cell(stream, cmake_generator);
        write_html_table_row_end(stream);

        write_html_table_row_start(stream);
        write_html_table_cell(stream, "Compiler:");
        write_html_table_cell(
            stream,
            std::format(
                "{0} {1} ({2})", cmake_cxx_compiler_id,
                cmake_cxx_compiler_version,
                cmake_cxx_compiler_architecture_id));
        write_html_table_row_end(stream);

        write_html_table_end(stream);

        write_html_section_end(stream);

        write_html_section_start(
            stream, html_section_heading::h3, "Configuration");

        write_html_table_start(stream);

        write_html_table_row_start(stream);
        write_html_table_cell(stream, "Build Type:");
        write_html_table_cell(stream, cmake_build_type);
        write_html_table_row_end(stream);

        write_html_table_row_start(stream);
        write_html_table_cell(stream, "C++ Flags:");
        write_html_table_cell(stream, cmake_cxx_flags);
        write_html_table_row_end(stream);

        write_html_table_row_start(stream);
        write_html_table_cell(stream, "C++ Standard:");
        write_html_table_cell(stream, cmake_cxx_standard);
        write_html_table_row_end(stream);

        write_html_table_row_start(stream);
        write_html_table_cell(stream, "Install Prefix:");
        write_html_table_cell(stream, cmake_install_prefix);
        write_html_table_row_end(stream);

        write_html_table_end(stream);

        write_html_section_end(stream);

        write_html_end(stream);

        return result;
    }

    ::QString
    compose_license_markup()
    {
        ::QString result;
        ::QTextStream stream(&result);

        write_html_start(stream);

        write_html_section_start(
            stream, html_section_heading::h3, "BSD-3-Clause");
        write_html_markdown(
            stream,
            ::QString(load_qresource_bytes(":/synthclone.app/LICENSE.md")));
        write_html_section_end(stream);

        write_html_end(stream);

        return result;
    }


    std::unique_ptr<::QDialog>
    build_about_dialog(Ui::about_dialog& dialog_state)
    {
        auto dialog = std::make_unique<::QDialog>();

        dialog_state.setupUi(dialog.get());

        dialog_state.application_tab_pane->setText(
            compose_application_markup());
        dialog_state.build_tab_pane->setText(compose_build_markup());
        dialog_state.license_tab_pane->setText(compose_license_markup());

        return dialog;
    }

    export
    class about_view final: private nonmovable {

    public:

        explicit
        about_view():
            dialog_(build_about_dialog(dialog_state_))
        {
            // empty
        }

        std::generator<event_wait_message>
        run(::QWidget* parent)
        {
            bool closed = false;
            int result;

            qobject_parent_guard parent_guard(dialog_.get(), parent);
            qobject_connection_guard connection_guard(
                dialog_.get(), &::QDialog::finished,
                [&closed, &result](int r) noexcept {
                    closed = true;
                    result = r;
                });
            qwidget_visibility_guard visibility_guard(dialog_.get(), true);

            while (! closed) {
                co_yield event_wait_message();
            }
        }

    private:

        Ui::about_dialog dialog_state_;
        std::unique_ptr<::QDialog> dialog_;

    };

}
