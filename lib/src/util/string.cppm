/**
 * @file
 *
 * Contains additional string functionality not provided by the standard
 * library.
 */

module;

#include <synthclone/config.h>

export module synthclone.util:string;

import std;

import :functional;
import :ranges;

///////////////////////////////////////////////////////////////////////////////
// synthclone::basic_string_proxy
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    export
    template<class T, class Char, class Traits, class Allocator>
    class basic_string_proxy;

    template<class T, class = void>
    struct is_string_proxy_type: std::false_type {};

    template<class T>
    struct is_string_proxy_type<
        T,
        std::void_t<
            std::enable_if_t<
                std::is_base_of_v<
                    basic_string_proxy<
                        T, typename T::value_type, typename T::traits_type,
                        typename T::allocator_type
                    >,
                    T
                >
            >
        >
    >: std::true_type {};

    template<class T>
    concept string_proxy_type = is_string_proxy_type<T>::value;

    /**
     * Read-only wrapper around a `std::basic_string` instance that's meant to
     * be extended by classes that want to provide a string API for their
     * class, but have stricter validation requirements than those specified by
     * `std::basic_string`.
     *
     * @tparam T
     *   The derived type.
     * @tparam Char
     *   The character type.
     * @tparam Traits
     *   Specifies basic operations on the character type.
     * @tparam Allocator
     *   Specifies the allocator used to allocate internal string storage.
     */

    export
    template<
        class T,
        class Char,
        class Traits = std::char_traits<Char>,
        class Allocator = std::allocator<Char>
    >
    class basic_string_proxy {

    public:

        /**
         * The proxied string type.
         */

        using string_type = std::basic_string<Char, Traits, Allocator>;

        /**
         * The type of allocator used to allocate string storage.
         */

        using allocator_type = string_type::allocator_type;

        /**
         * Iterator used to iterate over characters in the string.
         */

        using const_iterator = string_type::const_iterator;

        /**
         * Character pointer type.
         */

        using const_pointer = string_type::const_pointer;

        /**
         * Characer reference type.
         */

        using const_reference = string_type::const_reference;

        /**
         * Iterator used to iterate over characters in the string in reverse
         * order.
         */

        using const_reverse_iterator = string_type::const_reverse_iterator;

        /**
         * Type used to measure the difference between two iterators.
         */

        using difference_type = string_type::difference_type;

        /**
         * Iterator used to iterate over characters in the string.
         */

        using iterator = string_type::const_iterator;

        /**
         * Character pointer type.
         */

        using pointer = string_type::pointer;

        /**
         * Characer reference type.
         */

        using reference = string_type::reference;

        /**
         * Iterator used to iterate over characters in the string in reverse
         * order.
         */

        using reverse_iterator = string_type::const_reverse_iterator;

        /**
         * Type used to represent the length of the string.
         */

        using size_type = string_type::size_type;

        /**
         * Traits type used by the proxied string.
         */

        using traits_type = string_type::traits_type;

        /**
         * The character type.
         */

        using value_type = string_type::value_type;

        /**
         * Converts the `basic_string_proxy` instance to a
         * `std::basic_string_view`.
         *
         * @return
         *   The instantiated view object.
         */

        constexpr
        operator std::basic_string_view<value_type, traits_type>()
        const noexcept
        {
            return static_cast<
                std::basic_string_view<value_type, traits_type>
            >(s_);
        }

        /**
         * Converts the `basic_string_proxy` instance to a
         * `std::basic_string`.
         *
         * @return
         *   The instantiated string object.
         */

        constexpr
        operator string_type() const
        {
            return s_;
        }

        /**
         * Gets the character at the specified position.
         *
         * @param pos
         *   The position.
         *
         * @return
         *   The character.
         */

        constexpr
        const_reference
        operator[](size_type pos) const
        {
            return s_[pos];
        }

        /**
         * Gets the character at the specified position.
         *
         * @param pos
         *   The position.
         *
         * @return
         *   The character.
         */

        constexpr
        const_reference
        at(size_type pos) const
        {
            return s_.at(pos);
        }

        /**
         * Gets the last character in the string.
         *
         * @return
         *   The character.
         */

        constexpr
        const_reference
        back() const
        {
            return s_.back();
        }

        /**
         * Gets the proxied string.
         *
         * @return
         *   The string.
         */

        constexpr
        const string_type&
        base() const noexcept
        {
            return s_;
        }

        /**
         * Gets an iterator pointing at the beginning of the string.
         *
         * @return
         *   The iterator.
         */

        constexpr
        const_iterator
        begin() const noexcept
        {
            return s_.cbegin();
        }

        /**
         * Gets an iterator pointing at the beginning of the string.
         *
         * @return
         *   The iterator.
         */

        constexpr
        const_iterator
        cbegin() const noexcept
        {
            return s_.cbegin();
        }

        /**
         * Gets an iterator pointing at the end of the string.
         *
         * @return
         *   The iterator.
         */

        constexpr
        const_iterator
        cend() const noexcept
        {
            return s_.cend();
        }

        /**
         * Gets a reverse iterator pointing at the end of the string.
         *
         * @return
         *   The iterator.
         */

        constexpr
        const_reverse_iterator
        crbegin() const noexcept
        {
            return s_.crbegin();
        }

        /**
         * Gets a reverse iterator pointing at the beginning of the string.
         *
         * @return
         *   The iterator.
         */

        constexpr
        const_reverse_iterator
        crend() const noexcept
        {
            return s_.crend();
        }

        /**
         * Gets a pointer to the raw string data.
         *
         * @return
         *   The pointer.
         */

        constexpr
        const_pointer
        data() const noexcept
        {
            return s_.data();
        }

        /**
         * Gets a boolean indicating whether the string is empty.
         *
         * @return
         *   The boolean indicator.
         */

        constexpr
        bool
        empty() const noexcept
        {
            return s_.empty();
        }

        /**
         * Gets an iterator pointing at the end of the string.
         *
         * @return
         *   The iterator.
         */

        constexpr
        const_iterator
        end() const noexcept
        {
            return s_.cend();
        }

        /**
         * Gets the first character in the string.
         *
         * @return
         *   The character.
         */

        constexpr
        const_reference
        front() const
        {
            return s_.front();
        }

        /**
         * Gets the maximum possible string size.
         *
         * @return
         *   The maximum possible string size.
         */

        constexpr
        size_type
        max_size() const noexcept
        {
            return s_.max_size();
        }

        /**
         * Gets a reverse iterator pointing at the end of the string.
         *
         * @return
         *   The iterator.
         */

        constexpr
        const_reverse_iterator
        rbegin() const noexcept
        {
            return s_.crbegin();
        }

        /**
         * Gets a reverse iterator pointing at the beginning of the string.
         *
         * @return
         *   The iterator.
         */

        constexpr
        const_reverse_iterator
        rend() const noexcept
        {
            return s_.crend();
        }

        /**
         * Gets the string size.
         *
         * @return
         *   The size.
         */

        constexpr
        size_type
        size() const noexcept
        {
            return s_.size();
        }

        /**
         * Gets a boolean indicating if the two given strings are equal.
         *
         * @param lhs
         *   The first string.
         * @param rhs
         *   The second string.
         *
         * @return
         *   The boolean indicator.
         */

        constexpr friend
        bool
        operator==(const T& lhs, const T& rhs) noexcept
        {
            return lhs.base() == rhs.base();
        }

        /**
         * Gets a boolean indicating if the two given strings are equal.
         *
         * @param lhs
         *   The first string.
         * @param rhs
         *   The second string.
         *
         * @return
         *   The boolean indicator.
         */

        constexpr friend
        bool
        operator==(const T& lhs, const string_type& rhs) noexcept
        {
            return lhs.base() == rhs;
        }

        /**
         * Gets a boolean indicating if the two given strings are equal.
         *
         * @param lhs
         *   The first string.
         * @param rhs
         *   The second string.
         *
         * @return
         *   The boolean indicator.
         */

        constexpr friend
        bool
        operator==(const T& lhs, const Char* rhs) noexcept
        {
            return lhs.base() == rhs;
        }

        /**
         * Does a rich comparison of two strings.
         *
         * @param lhs
         *   The first string.
         * @param rhs
         *   The second string.
         *
         * @return
         *   The compare result.
         */

        constexpr friend
        auto
        operator<=>(const T& lhs, const T& rhs) noexcept
        {
            return lhs.base() <=> rhs.base();
        }

        /**
         * Does a rich comparison of two strings.
         *
         * @param lhs
         *   The first string.
         * @param rhs
         *   The second string.
         *
         * @return
         *   The compare result.
         */

        constexpr friend
        auto
        operator<=>(const T& lhs, const string_type& rhs) noexcept
        {
            return lhs.base() <=> rhs;
        }

        /**
         * Does a rich comparison of two strings.
         *
         * @param lhs
         *   The first string.
         * @param rhs
         *   The second string.
         *
         * @return
         *   The compare result.
         */

        constexpr friend
        auto
        operator<=>(const T& lhs, const Char* rhs) noexcept
        {
            return lhs.base() <=> rhs;
        }

        /**
         * Outputs the given string to the given stream instance.
         *
         * @param os
         *   The output stream instance.
         * @param s
         *   The string to output.
         *
         * @return
         *   A reference to the given output stream instance.
         */

        friend
        std::basic_ostream<Char>&
        operator<<(std::basic_ostream<Char>& os, const T& s)
        {
            return os << s.base();
        }

    protected:

        /**
         * Constructs a `basic_string_proxy` instance using the contents of the
         * given `basic_string_proxy` instance.
         *
         * @param s
         *   The instance to move.
         */

        constexpr
        basic_string_proxy(basic_string_proxy&& s) noexcept = default;

        /**
         * Constructs a `basic_string_proxy` instance using a copy of the
         * contents of the given `basic_string_proxy` instance.
         *
         * @param s
         *   The instance to copy.
         */

        constexpr
        basic_string_proxy(const basic_string_proxy& s) = default;

        /**
         * Constructs a `basic_string_proxy` instance by forwarding the given
         * arguments to the proxied string.
         *
         * @param args
         *   The arguments to forward.
         */

        template<class... Args>
        requires (std::constructible_from<string_type, Args...>)
        constexpr explicit
        basic_string_proxy(Args&&... args)
            noexcept(std::is_nothrow_constructible_v<string_type, Args...>):
            s_(std::forward<Args>(args)...)
        {
            // empty
        }

        /**
         * Destructs a `basic_string_proxy` instance.
         */

        constexpr
        ~basic_string_proxy() = default;

        /**
         * Assigns the contents of the given `basic_string_proxy` instance to
         * this `basic_string_proxy` instance.
         *
         * @param s
         *   The instance to move.
         *
         * @returns
         *   This instance.
         */

        constexpr
        basic_string_proxy&
        operator=(basic_string_proxy&& s) noexcept = default;

        /**
         * Copies the contents of the given `basic_string_proxy` instance to
         * this `basic_string_proxy` instance.
         *
         * @param s
         *   The instance to copy.
         *
         * @returns
         *   This instance.
         */

        constexpr
        basic_string_proxy&
        operator=(const basic_string_proxy& s) = default;

    private:

        string_type s_;

    };

    /**
     * String proxy type that iterates over `char` elements.
     *
     * @tparam T
     *   The derived type.
     */

    export
    template<class T>
    using string_proxy = basic_string_proxy<T, char>;

    /**
     * String proxy type that iterates over `char8_t` elements.
     *
     * @tparam T
     *   The derived type.
     */

    export
    template<class T>
    using u8string_proxy = basic_string_proxy<T, char8_t>;

    /**
     * String proxy type that iterates over `char16_t` elements.
     *
     * @tparam T
     *   The derived type.
     */

    export
    template<class T>
    using u16string_proxy = basic_string_proxy<T, char16_t>;

    /**
     * String proxy type that iterates over `char32_t` elements.
     *
     * @tparam T
     *   The derived type.
     */

    export
    template<class T>
    using u32string_proxy = basic_string_proxy<T, char32_t>;

    /**
     * String proxy type that iterates over `wchar_t` elements.
     *
     * @tparam T
     *   The derived type.
     */

    export
    template<class T>
    using wstring_proxy = basic_string_proxy<T, wchar_t>;

}

namespace std {

    /**
     * Disables the default range formatter for `basic_string_proxy` types so
     * that we're not dealing with more than one partial `formatter`
     * specialization.
     */

    export
    template<synthclone::string_proxy_type T>
    constexpr inline
    range_format format_kind<T> = range_format::disabled;

    /**
     * Formatter implementation for `basic_string_proxy` instances.
     */

    template<synthclone::string_proxy_type T, class Char>
    requires (formattable<typename T::string_type, Char>)
    class formatter<T, Char> {

    public:

        /**
         * Formats a `synthclone::basic_string_proxy` instance for output.
         *
         * @param s
         *   The `synthclone::basic_string_proxy` instance to format.
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
            return impl_.format(proxy.base(), context);
        }

        /**
         * Parses the format specification for a
         * `synthclone::basic_string_proxy` instance.
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

        formatter<typename T::string_type, Char> impl_;

    };

    /**
     * Hash implementation for `basic_string_proxy` instances.
     */

    template<synthclone::string_proxy_type T>
    requires (synthclone::hashable<typename T::string_type>)
    class hash<T> {

    public:

        /**
         * Computes the hash value for the given `basic_string_proxy` instance.
         *
         * @return
         *   The hash value.
         */

        std::size_t
        operator()(const T& proxy) const noexcept
        {
            return impl_(proxy.base());
        }

    private:

        hash<typename T::string_type> impl_;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::string_view_convertible
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Tests whether or not a given type can be converted to a
     * `basic_string_view` type with the given character and traits types.
     *
     * @tparam T
     *   The type to check.
     * @tparam Char
     *   The character type.
     * @tparam Traits
     *   The traits type.
     */

    export
    template<
        class T,
        class Char = char,
        class Traits = std::char_traits<Char>
    >
    concept string_view_convertible =
        std::convertible_to<T, std::basic_string_view<Char, Traits>>;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::string_convertible
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Tests whether or not a given type can be converted to a `basic_string`
     * type with the given character, traits, and allocator types.
     *
     * @tparam T
     *   The type to check.
     * @tparam Char
     *   The character type.
     * @tparam Traits
     *   The traits type.
     * @tparam Allocator
     *   The allocator type.
     */

    export
    template<
        class T,
        class Char = char,
        class Traits = std::char_traits<Char>,
        class Allocator = std::allocator<Char>
    >
    concept string_convertible =
        std::convertible_to<T, std::basic_string<Char, Traits, Allocator>>;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::concat_strings
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    template<class T, class Char, class Traits>
    concept string_concat_range =
        container_compatible_range<T, Char> ||
        string_view_convertible<T, Char, Traits>;

    template<
        class Char,
        class Traits,
        class Allocator,
        container_compatible_range<Char>... Args
    >
    requires (sizeof...(Args) >= 2)
    constexpr
    std::basic_string<Char, Traits, Allocator>
    concat_string_ranges(Args&&... args)
    {
        std::basic_string<Char, Traits, Allocator> result;
        result.reserve((... + args.size()));

        // libstdc++ doesn't support std::string::append_range() yet.
        //(result.append_range(std::forward<Args>(args)), ...);
        (result.append(args.begin(), args.end()), ...);

        return result;
    }

    template<class Char, class Traits, std::size_t N>
    constexpr
    std::basic_string_view<Char, Traits>
    make_concat_string_range(const Char (&array)[N])
    {
        return std::basic_string_view<Char, Traits>(
            array, std::find(array, array + N, '\0'));
    }

    template<class Char, class Traits, container_compatible_range<Char> T>
    constexpr
    decltype(auto)
    make_concat_string_range(T&& range)
    {
        return std::forward<T>(range);
    }

    template<
        class Char,
        class Traits,
        string_view_convertible<Char, Traits> T
    >
    requires (! container_compatible_range<T, Char>)
    constexpr
    std::basic_string_view<Char, Traits>
    make_concat_string_range(T&& range)
    {
        return std::forward<T>(range);
    }

    /**
     * Concatenates two or more strings.
     *
     * @tparam Char
     *   The character type.
     * @tparam Traits
     *   The character traits type.
     * @tparam Allocator
     *   The allocator type.
     *
     * @param s1
     *   The first string.
     * @param s2
     *   The second string.
     * @param args
     *   Additional strings.
     *
     * @returns
     *   The concatenated result.
     */

    export template<
        class Char = char,
        class Traits = std::char_traits<Char>,
        class Allocator = std::allocator<Char>,
        string_concat_range<Char, Traits>... Args
    >
    requires (sizeof...(Args) >= 2)
    constexpr
    std::basic_string<Char, Traits, Allocator>
    concat_strings(Args&&... args)
    {
        return concat_string_ranges<Char, Traits, Allocator>(
            make_concat_string_range<Char, Traits>(
                std::forward<Args>(args))...);
    }

}
