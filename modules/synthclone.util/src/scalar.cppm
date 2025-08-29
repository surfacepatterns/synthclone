/**
 * @file
 *
 * Contains additional scalar functionality not provided by the standard
 * library.
 */

module;

#include <synthclone/config.h>

export module synthclone.util:scalar;

import std;

import :concepts;
import :functional;

///////////////////////////////////////////////////////////////////////////////
// synthclone::basic_scalar_proxy
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    template<class T>
    concept streamable_scalar =
        scalar<T> &&
        requires (std::ostream& stream, T value) {
            { stream << value } -> std::convertible_to<std::ostream&>;
        };

    export
    template<class T, scalar S>
    class basic_scalar_proxy;

    template<class T, class = void>
    struct is_scalar_proxy_type: std::false_type {};

    template<class T>
    struct is_scalar_proxy_type<
        T,
        std::void_t<
            std::enable_if_t<
                std::is_base_of_v<
                    basic_scalar_proxy<T, typename T::scalar_type>, T
                >
            >
        >
    >: std::true_type {};

    template<class T>
    concept scalar_proxy_type = is_scalar_proxy_type<T>::value;

    /**
     * Read-only wrapper around a scalar value that's meant to be extended by
     * classes that want to provide a minimal scalar API for their class, but
     * have stricter requirements than those specified by the proxied scalar
     * type.
     *
     * @tparam T
     *   The derived type.
     * @tparam S
     *   The proxied scalar type.
     */

    export
    template<class T, scalar S>
    class basic_scalar_proxy {

    public:

        /**
         * The proxied scalar type.
         */

        using scalar_type = S;

        /**
         * Converts the proxied value to the given type.
         *
         * @tparam N
         *   The type to convert to.
         *
         * @return
         *   The value.
         */

        template<class N>
        requires (implicitly_convertible_to<S, N>)
        constexpr
        operator N() const noexcept
        {
            return value_;
        }

        /**
         * Converts the proxied value to the given type.
         *
         * @tparam N
         *   The type to convert to.
         *
         * @return
         *   The value.
         */

        template<class N>
        requires (
            (! implicitly_convertible_to<S, N>) &&
            explicitly_convertible_to<S, N>
        )
        constexpr explicit
        operator N() const noexcept
        {
            return static_cast<N>(value_);
        }

        /**
         * Gets a boolean indicating if the two given values are equal.
         *
         * @param lhs
         *   The first value.
         * @param rhs
         *   The second value.
         *
         * @return
         *   The boolean indicator.
         */

        constexpr friend
        bool
        operator==(T lhs, T rhs) noexcept
        {
            return static_cast<S>(lhs) == static_cast<S>(rhs);
        }

        /**
         * Gets a boolean indicating if the two given values are equal.
         *
         * @param lhs
         *   The first value.
         * @param rhs
         *   The second value.
         *
         * @return
         *   The boolean indicator.
         */

        constexpr friend
        bool
        operator==(T lhs, S rhs) noexcept
        {
            return static_cast<S>(lhs) == rhs;
        }

        /**
         * Does a rich comparison of two values.
         *
         * @param lhs
         *   The first value.
         * @param rhs
         *   The second value.
         *
         * @return
         *   The compare result.
         */

        constexpr friend
        auto
        operator<=>(T lhs, T rhs) noexcept
        {
            return static_cast<S>(lhs) <=> static_cast<S>(rhs);
        }

        /**
         * Does a rich comparison of two values.
         *
         * @param lhs
         *   The first value.
         * @param rhs
         *   The second value.
         *
         * @return
         *   The compare result.
         */

        constexpr friend
        auto
        operator<=>(T lhs, S rhs) noexcept
        {
            return static_cast<S>(lhs) <=> rhs;
        }

        /**
         * Outputs the given value to the given stream instance.
         *
         * @param os
         *   The output stream instance.
         * @param i
         *   The value to output.
         *
         * @return
         *   A reference to the given output stream instance.
         */

        template<class Char>
        requires (streamable_scalar<S>)
        friend
        std::basic_ostream<Char>&
        operator<<(std::basic_ostream<Char>& os, T s)
        {
            return os << static_cast<S>(s);
        }

        /**
         * Gets the proxied value.
         *
         * @return
         *   The proxied value.
         */

        constexpr
        S
        value() const noexcept
        {
            return value_;
        }

    protected:

        /**
         * Constructs an `basic_scalar_proxy` instance using a copy of the
         * contents of the given `basic_scalar_proxy` instance.
         *
         * @param i
         *   The instance to copy.
         */

        constexpr
        basic_scalar_proxy(const basic_scalar_proxy& i) noexcept = default;

        /**
         * Constructs a `basic_scalar_proxy` using the given value.
         *
         * @param n
         *   The value.
         */

        constexpr
        basic_scalar_proxy(S n) noexcept:
            value_(n)
        {
            // empty
        }

        /**
         * Destructs a `basic_scalar_proxy` instance.
         */

        constexpr
        ~basic_scalar_proxy() noexcept = default;

        /**
         * Copies the contents of the given `basic_scalar_proxy` instance to
         * this `basic_scalar_proxy` instance.
         *
         * @param s
         *   The instance to copy.
         *
         * @returns
         *   This instance.
         */

        constexpr
        basic_scalar_proxy&
        operator=(const basic_scalar_proxy&) = default;

    private:

        S value_;

    };

    /**
     * Scalar proxy type that contains a `bool`.
     *
     * @tparam T
     *   The derived type.
     */

    export
    template<class T>
    using bool_proxy = basic_scalar_proxy<T, bool>;

    /**
     * Scalar proxy type that contains a `char`.
     *
     * @tparam T
     *   The derived type.
     */

    export
    template<class T>
    using char_proxy = basic_scalar_proxy<T, char>;

    /**
     * Scalar proxy type that contains a `char8_t`.
     *
     * @tparam T
     *   The derived type.
     */

    export
    template<class T>
    using char8_proxy = basic_scalar_proxy<T, char8_t>;

    /**
     * Scalar proxy type that contains a `char16_t`.
     *
     * @tparam T
     *   The derived type.
     */

    export
    template<class T>
    using char16_proxy = basic_scalar_proxy<T, char16_t>;

    /**
     * Scalar proxy type that contains a `char32_t`.
     *
     * @tparam T
     *   The derived type.
     */

    export
    template<class T>
    using char32_proxy = basic_scalar_proxy<T, char32_t>;

    /**
     * Scalar proxy type that contains a `wchar_t`.
     *
     * @tparam T
     *   The derived type.
     */

    export
    template<class T>
    using wchar_proxy = basic_scalar_proxy<T, wchar_t>;

    /**
     * Scalar proxy type that contains a `short`.
     *
     * @tparam T
     *   The derived type.
     */

    export
    template<class T>
    using short_proxy = basic_scalar_proxy<T, short>;

    /**
     * Scalar proxy type that contains an `int`.
     *
     * @tparam T
     *   The derived type.
     */

    export
    template<class T>
    using int_proxy = basic_scalar_proxy<T, int>;

    /**
     * Scalar proxy type that contains a `long`.
     *
     * @tparam T
     *   The derived type.
     */

    export
    template<class T>
    using long_proxy = basic_scalar_proxy<T, long>;

    /**
     * Scalar proxy type that contains a `long long`.
     *
     * @tparam T
     *   The derived type.
     */

    export
    template<class T>
    using long_long_proxy = basic_scalar_proxy<T, long long>;

    /**
     * Scalar proxy type that contains an `unsigned char`.
     *
     * @tparam T
     *   The derived type.
     */

    export
    template<class T>
    using unsigned_char_proxy = basic_scalar_proxy<T, unsigned char>;

    /**
     * Scalar proxy type that contains an `unsigned short`.
     *
     * @tparam T
     *   The derived type.
     */

    export
    template<class T>
    using unsigned_short_proxy = basic_scalar_proxy<T, unsigned short>;

    /**
     * Scalar proxy type that contains an `unsigned int`.
     *
     * @tparam T
     *   The derived type.
     */

    export
    template<class T>
    using unsigned_int_proxy = basic_scalar_proxy<T, unsigned int>;

    /**
     * Scalar proxy type that contains an `unsigned long`.
     *
     * @tparam T
     *   The derived type.
     */

    export
    template<class T>
    using unsigned_long_proxy = basic_scalar_proxy<T, unsigned long>;

    /**
     * Scalar proxy type that contains an `unsigned long long`.
     *
     * @tparam T
     *   The derived type.
     */

    export
    template<class T>
    using unsigned_long_long_proxy = basic_scalar_proxy<T, unsigned long long>;

    /**
     * Scalar proxy type that contains an `std::int_least8_t`.
     *
     * @tparam T
     *   The derived type.
     */

    export
    template<class T>
    using int_least8_proxy = basic_scalar_proxy<T, std::int_least8_t>;

    /**
     * Scalar proxy type that contains an `std::int_least16_t`.
     *
     * @tparam T
     *   The derived type.
     */

    export
    template<class T>
    using int_least16_proxy = basic_scalar_proxy<T, std::int_least16_t>;

    /**
     * Scalar proxy type that contains an `std::int_least32_t`.
     *
     * @tparam T
     *   The derived type.
     */

    export
    template<class T>
    using int_least32_proxy = basic_scalar_proxy<T, std::int_least32_t>;

    /**
     * Scalar proxy type that contains an `std::int_least64_t`.
     *
     * @tparam T
     *   The derived type.
     */

    export
    template<class T>
    using int_least64_proxy = basic_scalar_proxy<T, std::int_least64_t>;

    /**
     * Scalar proxy type that contains an `std::uint_least8_t`.
     *
     * @tparam T
     *   The derived type.
     */

    export
    template<class T>
    using uint_least8_proxy = basic_scalar_proxy<T, std::uint_least8_t>;

    /**
     * Scalar proxy type that contains an `std::uint_least16_t`.
     *
     * @tparam T
     *   The derived type.
     */

    export
    template<class T>
    using uint_least16_proxy = basic_scalar_proxy<T, std::uint_least16_t>;

    /**
     * Scalar proxy type that contains an `std::uint_least32_t`.
     *
     * @tparam T
     *   The derived type.
     */

    export
    template<class T>
    using uint_least32_proxy = basic_scalar_proxy<T, std::uint_least32_t>;

    /**
     * Scalar proxy type that contains an `std::uint_least64_t`.
     *
     * @tparam T
     *   The derived type.
     */

    export
    template<class T>
    using uint_least64_proxy = basic_scalar_proxy<T, std::uint_least64_t>;

    /**
     * Scalar proxy type that contains a `float`.
     *
     * @tparam T
     *   The derived type.
     */

    export
    template<class T>
    using float_proxy = basic_scalar_proxy<T, float>;

    /**
     * Scalar proxy type that contains a `double`.
     *
     * @tparam T
     *   The derived type.
     */

    export
    template<class T>
    using double_proxy = basic_scalar_proxy<T, double>;

    /**
     * Scalar proxy type that contains a `long double`.
     *
     * @tparam T
     *   The derived type.
     */

    export
    template<class T>
    using long_double_proxy = basic_scalar_proxy<T, long double>;

    /**
     * Scalar proxy type that contains a `std::byte`.
     *
     * @tparam T
     *   The derived type.
     */

    export
    template<class T>
    using byte_proxy = basic_scalar_proxy<T, std::byte>;

}

namespace std {

    /**
     * Formatter implementation for `basic_scalar_proxy` instances that contain
     * scalar types that are themselves formattable.
     */

    template<synthclone::scalar_proxy_type T, class Char>
    requires (formattable<typename T::scalar_type, Char>)
    class formatter<T, Char> {

    public:

        /**
         * Formats a `synthclone::basic_scalar_proxy` instance for output.
         *
         * @param i
         *   The `synthclone::basic_scalar_proxy` instance to format.
         * @param context
         *   The format context to write the output to.
         *
         * @return
         *   An iterator pointing at the end of the output range.
         */

        template<class FormatContext>
        typename FormatContext::iterator
        format(const T& proxy, FormatContext& context) const
        {
            return impl_.format(proxy.value(), context);
        }

        /**
         * Parses the format specification for a
         * `synthclone::basic_scalar_proxy` instance.
         *
         * @param context
         *   The parse context.
         *
         * @return
         *   An iterator pointing at the end of the parsed specification.
         */

        template<class ParseContext>
        constexpr
        typename ParseContext::iterator
        parse(ParseContext& context)
        {
            return impl_.parse(context);
        }

    private:

        formatter<typename T::scalar_type, Char> impl_;

    };

    /**
     * Hash implementation for `basic_scalar_proxy` instances that contain
     * scalar types that are themselves hashable.
     */

    template<synthclone::scalar_proxy_type T>
    requires (synthclone::hashable<typename T::scalar_type>)
    class hash<T> {

    public:

        /**
         * Computes the hash value for the given `basic_scalar_proxy`
         * instance.
         *
         * @return
         *   The hash value.
         */

        std::size_t
        operator()(const T& proxy) const noexcept
        {
            return impl_(proxy.value());
        }

    private:

        hash<typename T::scalar_type> impl_;

    };

}
