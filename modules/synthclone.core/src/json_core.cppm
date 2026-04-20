/**
 * @file
 *
 * Contains (mostly internal) functionality on top of `boost.json` for
 * validating parsed JSON.
 */

module;

#include <synthclone/config.h>

export module synthclone.core:json_core;

import std;

import synthclone.external.boost.core;
import synthclone.external.boost.json;
import synthclone.external.boost.mp11;
import synthclone.util;

///////////////////////////////////////////////////////////////////////////////
// synthclone::json_path_segment_type
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    export
    template<class T>
    concept json_path_segment_type =
        std::same_as<T, std::size_t> || std::same_as<T, std::string_view>;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::json_path_type
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    export
    template<json_path_segment_type... S>
    class json_path;

    template<class T>
    struct is_json_path_type: std::false_type {};

    template<json_path_segment_type... S>
    struct is_json_path_type<json_path<S...>>: std::true_type {};

    export
    template<class T>
    concept json_path_type = is_json_path_type<T>::value;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::json_path_size_v
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    template<class T>
    struct json_path_size;

    template<json_path_segment_type... Types>
    struct json_path_size<json_path<Types...>>:
        std::integral_constant<std::size_t, sizeof...(Types)> {};

    export
    template<json_path_type T>
    constexpr std::size_t json_path_size_v = json_path_size<T>::value;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::json_path
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    template<class FormatContext>
    void
    format_json_path_segment(std::size_t n, FormatContext& context)
    {
        context.advance_to(std::format_to(context.out(), "["));
        context.advance_to(std::formatter<std::size_t>{}.format(n, context));
        context.advance_to(std::format_to(context.out(), "]"));
    }

    template<class FormatContext>
    void
    format_json_path_segment(std::string_view s, FormatContext& context)
    {
        context.advance_to(std::format_to(context.out(), "."));
        context.advance_to(
            std::formatter<std::string_view>{}.format(s, context));
    }

    template<json_path_type T, class FormatContext, std::size_t... Indices>
    typename FormatContext::iterator
    format_json_path(
        const T& path,
        FormatContext& context,
        std::index_sequence<Indices...>
    )
    {
        context.advance_to(std::format_to(context.out(), "$"));
        (format_json_path_segment(path.template get<Indices>(), context), ...);
        return context.out();
    }

    export
    template<json_path_segment_type... S>
    class json_path final {

    public:

        template<class... Args>
        requires (
            (sizeof...(Args) == sizeof...(S)) &&
            (std::same_as<S, std::remove_cvref_t<Args>> && ...)
        )
        constexpr explicit
        json_path(Args&&... segments):
            segments_(std::forward<Args>(segments)...)
        {
            // empty
        }

        template<std::size_t I>
        constexpr
        const typename std::tuple_element<I, std::tuple<S...>>::type&
        get() const noexcept
        {
            return std::get<I>(segments_);
        }

    private:

        std::tuple<S...> segments_;

    };

    export
    template<class... S>
    requires (json_path_segment_type<std::remove_cvref_t<S>> && ...)
    json_path(S...) -> json_path<std::remove_cvref_t<S>...>;

    template<json_path_segment_type... S, class T, std::size_t... Indices>
    requires (json_path_segment_type<std::remove_cvref_t<T>>)
    constexpr
    json_path<S..., std::remove_cvref_t<T>>
    make_json_path(
        const json_path<S...>& parent_path,
        T&& element,
        std::index_sequence<Indices...>
    )
    {
        return json_path<S..., std::remove_cvref_t<T>>{
            parent_path.template get<Indices>()..., element};
    }

    template<json_path_segment_type... S, class T>
    requires (json_path_segment_type<std::remove_cvref_t<T>>)
    constexpr
    json_path<S..., std::remove_cvref_t<T>>
    make_json_path(const json_path<S...>& parent_path, T&& element)
    {
        return make_json_path(
            parent_path, std::forward<T>(element),
            std::make_index_sequence<sizeof...(S)>{});
    }

}

namespace std {

    export
    template<synthclone::json_path_type T, class Char>
    class formatter<T, Char> {

    public:

        template<class FormatContext>
        typename FormatContext::iterator
        format(const T& path, FormatContext& context) const
        {
            return synthclone::format_json_path(
                path, context,
                std::make_index_sequence<synthclone::json_path_size_v<T>>{});
        }

        template<class ParseContext>
        constexpr
        typename ParseContext::iterator
        parse(ParseContext& context)
        {
            return context.begin();
        }

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::json_error
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    export
    class json_error: public std::runtime_error {

    public:

        using std::runtime_error::runtime_error;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::json_traversal_error
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Identifies an error that occurred during traversal of a JSON tree.
     */

    export
    class json_traversal_error: public json_error {

    public:

        /**
         * Constructs a `json_traversal_error` instance.
         *
         * @param path
         *   The traversal path where the error occurred.
         * @param message
         *   The error message.
         */

        inline
        json_traversal_error(std::string path, const std::string& message):
            json_error(std::format("{0}: {1}", path, message)),
            path_(std::move(path))
        {
            // empty
        }

        /**
         * Constructs a `json_traversal_error` instance.
         *
         * @param path
         *   The traversal path where the error occurred.
         * @param message
         *   The error message.
         */

        inline
        json_traversal_error(std::string path, const char* message):
            json_error(std::format("{0}: {1}", path, message)),
            path_(std::move(path))
        {
            // empty
        }

        /**
         * Gets the traversal path.
         *
         * @return
         *   The path.
         */

        const std::string&
        path() const noexcept
        {
            return path_;
        }

    private:

        std::string path_;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::json_validation_error
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    export
    class json_validation_error: public json_error {

    public:

        using json_error::json_error;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::json_integral_type
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    export
    template<class T>
    concept json_integral_type =
        std::same_as<T, std::int64_t> || std::same_as<T, std::uint64_t>;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::json_number_type
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    export
    template<class T>
    concept json_number_type =
        std::same_as<T, double> || json_integral_type<T>;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::basic_json_node
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    export
    template<json_path_type P, class T>
    class basic_json_node final {

    public:

        constexpr
        basic_json_node(P path, const T& element) noexcept:
            path_(std::move(path)),
            element_(element)
        {
            // empty
        }

        constexpr
        const T&
        element() const noexcept
        {
            return element_.get();
        }

        constexpr
        const P&
        path() const noexcept
        {
            return path_;
        }

    private:

        P path_;
        std::reference_wrapper<const T> element_;

    };

    export
    template<json_path_type P, class T>
    basic_json_node(P, T) -> basic_json_node<P, T>;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::json_array_node
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    export
    template<json_path_type P>
    using json_array_node = basic_json_node<P, boost::json::array>;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::json_boolean_node
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    export
    template<json_path_type P>
    using json_boolean_node = basic_json_node<P, bool>;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::json_null_node
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    export
    template<json_path_type P>
    using json_null_node = basic_json_node<P, std::nullptr_t>;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::json_number_node
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    export
    template<json_number_type T, json_path_type P>
    using json_number_node = basic_json_node<P, T>;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::json_object_node
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    export
    template<json_path_type P>
    using json_object_node = basic_json_node<P, boost::json::object>;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::json_string_node
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    export
    template<json_path_type P>
    using json_string_node = basic_json_node<P, boost::json::string>;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::traverse()
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    using json_variant_list = boost::mp11::mp_list<
        boost::json::array,
        boost::json::object,
        boost::json::string,
        bool,
        std::nullptr_t,
        double,
        std::int64_t,
        std::uint64_t
    >;

    template<class F, json_path_type P>
    using json_traverse_result_t = typename boost::mp11::mp_apply<
        std::common_type,
        boost::mp11::mp_transform_q<
            boost::mp11::mp_bind<
                std::invoke_result_t,
                F,
                boost::mp11::mp_arg<0>
            >,
            boost::mp11::mp_filter_q<
                boost::mp11::mp_bind<
                    std::is_invocable,
                    F,
                    boost::mp11::mp_arg<0>
                >,
                boost::mp11::mp_transform_q<
                    boost::mp11::mp_bind<
                        basic_json_node,
                        P,
                        boost::mp11::mp_arg<0>
                    >,
                    json_variant_list
                >
            >
        >
    >::type;

    template<json_path_type P, class F>
    class json_traverse_visitor final {

    public:

        constexpr explicit
        json_traverse_visitor(const P& path, F&& f) noexcept:
            path_(path),
            f_(std::forward<F>(f))
        {
            // empty
        }

        template<class T>
        [[noreturn]]
        json_traverse_result_t<F, P>
        operator()(const T& element)
        requires (! std::invocable<F, const basic_json_node<P, T>&>)
        {
            throw json_traversal_error(
                std::format("{0}", path_),
                std::format(
                    "value is of unexpected type {0}",
                    boost::core::demangle(typeid(T).name())));
        }

        template<class T>
        json_traverse_result_t<F, P>
        operator()(const T& element)
        requires (std::invocable<F, const basic_json_node<P, T>&>)
        {
            const basic_json_node<P, T> node(path_, element);
            try {
                return f_(node);
            } catch (const json_validation_error& e) {
                std::throw_with_nested(
                    json_traversal_error(std::format("{0}", path_), e.what()));
            }
        }

    private:

        const P& path_;
        F&& f_;

    };

    export
    template<json_path_segment_type... S, class F>
    json_traverse_result_t<F, json_path<S..., std::size_t>>
    traverse(
        const json_array_node<json_path<S...>>& node,
        std::size_t index,
        F&& f
    )
    {
        const auto &element = node.element();

        const auto size = element.size();
        verify(
            index < size, "index {0} is out of bounds for array of size {1}",
            index, size);

        return boost::json::visit(
            json_traverse_visitor<json_path<S..., std::size_t>, F>(
                make_json_path(node.path(), index), std::forward<F>(f)),
            element[index]);
    }

    export
    template<json_path_segment_type... S, class F>
    json_traverse_result_t<F, json_path<S..., std::string_view>>
    traverse(
        const json_object_node<json_path<S...>>& node,
        std::string_view key,
        F&& f
    )
    {
        const auto& element = node.element();
        const auto& path = node.path();

        auto iter = element.find(key);
        if (iter == element.end()) [[unlikely]] {
            throw json_traversal_error(
                std::format("{0}", path),
                std::format("required key {0:?} is missing", key));
        }

        return boost::json::visit(
            json_traverse_visitor<json_path<S..., std::string_view>, F>(
                make_json_path(path, key), std::forward<F>(f)),
            iter->value());
    }

    export
    template<class F>
    json_traverse_result_t<F, json_path<>>
    traverse(const boost::json::value& value, F&&f)
    {
        return boost::json::visit(
            json_traverse_visitor<json_path<>, F>(
                json_path<>(), std::forward<F>(f)),
            value);
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::traverse_if()
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    export
    template<json_path_segment_type... S, class F>
    std::optional<json_traverse_result_t<F, json_path<S..., std::string_view>>>
    traverse_if(
        const json_object_node<json_path<S...>>& node,
        std::string_view key,
        F&& f
    )
    {
        auto &element = node.element();
        auto iter = element.find(key);
        if (iter == element.end()) {
            return std::nullopt;
        }
        return boost::json::visit(
            json_traverse_visitor<json_path<S..., std::string_view>, F>(
                make_json_path(node.path(), key), std::forward<F>(f)),
            iter->value());
    }

}
