module;

export module synthclone.test:debug;

import std;

import synthclone.external.boost;
import synthclone.util;

import :concepts;

///////////////////////////////////////////////////////////////////////////////
// synthclone::make_test_info()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    class test_info final {

    public:

        template<class T>
        requires (string_view_convertible<const T&>)
        consteval
        test_info(
            const T& s,
            std::source_location location = std::source_location::current()
        ) noexcept:
            location_(std::move(location)),
            s_(s)
        {
            // empty
        }

        constexpr
        const std::source_location&
        location() const noexcept
        {
            return location_;
        }

        constexpr
        std::string_view
        string() const noexcept
        {
            return s_;
        }

    private:

        const std::source_location location_;
        const std::string_view s_;

    };

    template<string_view_convertible<char> T>
    std::string
    make_test_info_arg(const T& arg)
    {
        return std::format("{0:?}", static_cast<std::string_view>(arg));
    }

    template<std::formattable<char> T>
    requires (! string_view_convertible<T>)
    std::string
    make_test_info_arg(const T& arg)
    {
        return std::format("{0}", arg);
    }

    template<ostream_printable T>
    requires (! (string_view_convertible<T> || std::formattable<T, char>))
    std::string
    make_test_info_arg(const T& arg)
    {
        std::ostringstream stream;
        stream << arg;
        return std::move(stream).str();
    }

    template<class T>
    requires (
        ! (
            string_view_convertible<T> ||
            std::formattable<T, char> ||
            ostream_printable<T>
        )
    )
    std::string
    make_test_info_arg(const T& arg)
    {
        return std::format(
            "{0}@{1}", boost::core::demangle(typeid(T).name()),
            reinterpret_cast<std::uintptr_t>(std::addressof(arg)));
    }

    constexpr std::string_view test_info_args_delimiter(", ");

    export
    template<class... Args>
    std::string
    make_test_info(test_info info, Args&&... args)
    {
        const auto& location = info.location();
        return std::format(
            "{0}({1}) [{2}:{3}:{4}]", info.string(),
            join_strings(
                test_info_args_delimiter,
                make_test_info_arg(std::forward<Args>(args))...),
            location.file_name(), location.line(), location.column());
    }

}
