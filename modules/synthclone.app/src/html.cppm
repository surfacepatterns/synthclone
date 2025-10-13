export module synthclone.app:html;

import std;

import synthclone.external.qt.core;
import synthclone.external.qt.gui;
import synthclone.util;

///////////////////////////////////////////////////////////////////////////////
// synthclone::html_section_heading
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    enum class html_section_heading: std::uint_least8_t {
        h1 = 0,
        h2 = 1,
        h3 = 2,
        h4 = 3,
        h5 = 4,
        h6 = 5
    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::write_html_markdown()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    template<class T>
    concept qstring_convertible =
        std::convertible_to<T, ::QString> ||
        std::same_as<std::remove_cvref_t<T>, std::string> ||
        std::same_as<std::remove_cvref_t<T>, std::string_view>;

    template<class T>
    requires (std::constructible_from<::QString, T>)
    ::QString
    make_qstring(T&& s)
    {
        return ::QString(std::forward<T>(s));
    }

    ::QString
    make_qstring(const std::string& s)
    {
        return ::QString::fromStdString(s);
    }

    ::QString
    make_qstring(std::string_view s)
    {
        return ::QString::fromUtf8(s.data(), s.size());
    }

    export
    template<qstring_convertible T>
    void
    write_html_markdown(::QTextStream& stream, T&& s)
    {
        ::QTextDocument document;
        document.setMarkdown(make_qstring(s));
        stream << document.toHtml();
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::write_html_text()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    template<qstring_convertible T>
    void
    write_html_text(::QTextStream& stream, T&& s)
    {
        stream << make_qstring(std::forward<T>(s)).toHtmlEscaped();
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::write_html_start()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    inline
    void
    write_html_start(::QTextStream& stream)
    {
        stream << "<html><body>";
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::write_html_end()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    inline
    void
    write_html_end(::QTextStream& stream)
    {
        stream << "</body></html>";
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::write_html_section_start()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    constexpr lookup_table<::QStringView, 6>
    html_section_heading_start_tags {
        ::QStringView(u"<h1>"),
        ::QStringView(u"<h2>"),
        ::QStringView(u"<h3>"),
        ::QStringView(u"<h4>"),
        ::QStringView(u"<h5>"),
        ::QStringView(u"<h6>")
    };

    constexpr lookup_table<::QStringView, 6>
    html_section_heading_end_tags {
        ::QStringView(u"</h1>"),
        ::QStringView(u"</h2>"),
        ::QStringView(u"</h3>"),
        ::QStringView(u"</h4>"),
        ::QStringView(u"</h5>"),
        ::QStringView(u"</h6>")
    };

    export
    template<qstring_convertible T>
    void
    write_html_section_start(
        ::QTextStream& stream,
        html_section_heading level,
        T&& heading_text
    )
    {
        auto n = static_cast<std::size_t>(level);
        assume(
            n <= static_cast<std::size_t>(html_section_heading::h6),
            "{0}: unexpected `html_section_heading` value", n);

        stream << "<div class=\"section\">"
               << html_section_heading_start_tags[n];
        write_html_text(stream, heading_text);
        stream << html_section_heading_end_tags[n]
               << "<div class=\"section-content\">";
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::write_html_section_end()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    inline
    void
    write_html_section_end(::QTextStream& stream)
    {
        stream << "</div></div>";
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::write_html_table_start()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    inline
    void
    write_html_table_start(::QTextStream& stream)
    {
        stream << "<table>";
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::write_html_table_end()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    inline
    void
    write_html_table_end(::QTextStream& stream)
    {
        stream << "</table>";
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::write_html_table_row_start()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    inline
    void
    write_html_table_row_start(::QTextStream& stream)
    {
        stream << "<tr>";
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::write_html_table_row_end()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    inline
    void
    write_html_table_row_end(::QTextStream& stream)
    {
        stream << "</tr>";
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::write_html_table_cell()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    template<qstring_convertible T>
    void
    write_html_table_cell(::QTextStream& stream, T&& cell_text)
    {
        stream << "<td>";
        write_html_text(stream, std::forward<T>(cell_text));
        stream << "</td>";
    }

}
