/**
 * @file
 *
 * Contains unicode oriented functionality.
 *
 * @note
 *   The functionality in this module is mostly a thin wrapper around the
 *   `utf8proc` library.
 *
 *   XXX: We almost certainly want a `constexpr`-friendly `utf8_string` before
 *   release.  That would mean using another unicode library that is
 *   `constexpr`-friendly (does one actually exist?), or rolling out a minimal
 *   unicode parsing implementation using generated code from the unicode
 *   database.
 *
 *   This module only provides a small subset of unicode-oriented
 *   functionality; in particular, facilities to decode UTF-8 are provided so
 *   that UTF-8 string classes can perform validation.  In particular, you
 *   can't really count on UTF-8 encoded string comparisons for anything but
 *   byte-by-byte comparisons (which are still useful, but don't take things
 *   like unicode normalization forms into account).
 *
 *   The functionality here may very well be expanded in the future.
 */

module;

#include <boost/mp11/list.hpp>

#include <utf8proc.h>

#include <synthclone/config.h>

export module synthclone.util:unicode_core;

import std;

import :debug;
import :scalar;
import :string;
import :unicode_gen;
import :utility;

///////////////////////////////////////////////////////////////////////////////
// synthclone::unicode_error
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Exception class for unicode-specific errors.
     */

    export
    class unicode_error: public std::runtime_error {

    public:

        using std::runtime_error::runtime_error;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::unicode_codepoint
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    export
    class unicode_codepoint;

    export
    class utf8_decode_iterator;

    using utf8_byte_iterator = const std::byte*;

    char32_t
    verify_unicode_codepoint(const char32_t n)
    {
        verify(
            ::utf8proc_codepoint_valid(static_cast<::utf8proc_int32_t>(n)),
            "'{0}': invalid codepoint value",
            static_cast<std::int_least32_t>(n));
        return n;
    }

    /**
     * Contains a valid unicode codepoint.
     */

    export
    class unicode_codepoint final: public char32_proxy<unicode_codepoint> {

    public:

        /**
         * Constructs a `codepoint` instance.
         *
         * @param n
         *   The codepoint value.
         */

        unicode_codepoint(const char32_t n):
            char32_proxy<unicode_codepoint>(verify_unicode_codepoint(n))
        {
            // empty
        }

    private:

        friend class utf8_decode_iterator;

        constexpr
        unicode_codepoint(validated_t, const char32_t n):
            char32_proxy<unicode_codepoint>(n)
        {
            // empty
        }

    };

    /**
     * Gets the codepoint category.
     *
     * @param c
     *   The codepoint.
     *
     * @return
     *   The category.
     */

    export
    inline
    unicode_category
    get_category(unicode_codepoint c)
    noexcept
    {
        return static_cast<unicode_category>(
            ::utf8proc_category(static_cast<::utf8proc_int32_t>(c.value())));
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::utf8_decode_iterator
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    export
    class utf8_decode_view;

    ::utf8proc_int32_t
    decode_utf8_codepoint(utf8_byte_iterator& iter, utf8_byte_iterator end)
    {
        verify(end != nullptr, "iterator does not point at a codepoint span");
        verify(iter != nullptr, "there are no more codepoints to decode");

        if (iter == end) [[unlikely]] {
            iter = nullptr;
            return 0;
        }

        ::utf8proc_int32_t n;
        const auto size = end - iter;
        auto result = ::utf8proc_iterate(
            reinterpret_cast<const ::utf8proc_uint8_t*>(iter), size, &n);
        if (result < 0) [[unlikely]] {

            const auto* message = ::utf8proc_errmsg(result);
            assume(
                message != nullptr,
                "error message for error code {0} is set to NULL", result);

            throw unicode_error(
                std::format(
                    "::utf8proc_iterate({0:x}, {1}, {2:x}): failed to decode "
                    "codepoint: {3}",
                    reinterpret_cast<std::uintptr_t>(iter), size,
                    reinterpret_cast<std::uintptr_t>(&n), message));
        }

        iter += result;
        assume(
            iter <= end,
            "iterator ({0:x}) is passed end of byte range ({1:x})",
            reinterpret_cast<std::uintptr_t>(iter),
            reinterpret_cast<std::uintptr_t>(end));

        return n;
    }

    /**
     * Iterates over decoded codepoints in UTF-8 encoded data.
     */

    export
    class utf8_decode_iterator final {

    public:

        /**
         * The (unused) difference type.
         */

        using difference_type = std::ptrdiff_t;

        /**
         * The element type.
         */

        using value_type = unicode_codepoint;

        /**
         * Constructs an invalid iterator.
         */

        constexpr
        utf8_decode_iterator()
        noexcept:
            iter_(nullptr),
            end_(nullptr)
        {
            // empty
        }

        /**
         * Gets the current codepoint.
         *
         * @return
         *   The codepoint.
         */

        constexpr
        unicode_codepoint
        operator*()
        const
        {
            verify(iter_ != nullptr, "the iterator is exhausted");
            return unicode_codepoint(validated, static_cast<char32_t>(n_));
        }

        /**
         * Decodes the next codepoint and makes it the current codepoint if
         * there are more codepoints to decode; otherwise, marks the iterator
         * as exhausted.
         *
         * @return
         *   This iterator.
         */

        inline
        utf8_decode_iterator&
        operator++()
        {
            n_ = decode_utf8_codepoint(iter_, end_);
            return *this;
        }

        /**
         * Decodes the next codepoint and makes it the current codepoint if
         * there are more codepoints to decode; otherwise, marks the iterator
         * as exhausted.
         *
         * @return
         *   An iterator pointing at the pre-increment position.
         */

        inline
        utf8_decode_iterator
        operator++(int)
        {
            utf8_decode_iterator iter(*this);
            ++(*this);
            return iter;
        }

        /**
         * Gets a boolean indicating whether or not the two iterators point to
         * the same location.
         *
         * @param lhs
         *   The first iterator.
         * @param rhs
         *   The second iterator.
         *
         * @return
         *   The boolean indicator.
         */

        constexpr
        friend
        bool
        operator==(utf8_decode_iterator lhs, utf8_decode_iterator rhs)
        noexcept
        {
            return (lhs.iter_ == rhs.iter_) && (lhs.end_ == rhs.end_);
        }

        /**
         * Gets a boolean indicating whether or not the given iterator has
         * reached the end of the view.
         *
         * @param iter
         *   The iterator.
         * @param sentinel
         *   The sentinel.
         *
         * @return
         *   The boolean indicator.
         */

        constexpr
        friend
        bool
        operator==(utf8_decode_iterator iter, std::default_sentinel_t sentinel)
        noexcept
        {
            return iter.iter_ == nullptr;
        }

    private:

        friend class utf8_decode_view;

        utf8_decode_iterator(utf8_byte_iterator iter, utf8_byte_iterator end):
            iter_(iter),
            end_(end),
            n_(decode_utf8_codepoint(iter_, end_))
        {
            // empty
        }

        utf8_byte_iterator iter_;
        utf8_byte_iterator end_;
        ::utf8proc_int32_t n_;

    };

    /**
     * Gets a boolean indicating whether the two iterators point to different
     * locations.
     *
     * @param lhs
     *   The first iterator.
     * @param rhs
     *   The second iterator.
     *
     * @return
     *   The boolean indicator.
     */

    export
    constexpr
    bool
    operator!=(utf8_decode_iterator lhs, utf8_decode_iterator rhs)
    noexcept
    {
        return ! (lhs == rhs);
    }

    /**
     * Gets a boolean indicating whether or not the given iterator is not at
     * the end of the view.
     *
     * @param iter
     *   The iterator.
     * @param sentinel
     *   The sentinel.
     *
     * @return
     *   The boolean indicator.
     */

    export
    constexpr
    bool
    operator!=(utf8_decode_iterator iter, std::default_sentinel_t sentinel)
    noexcept
    {
        return ! (iter == sentinel);
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::utf8_decode_view
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * View that iteratively decodes UTF-8 encoded bytes into their
     * corresponding codepoints.
     */

    export
    class utf8_decode_view final:
        public std::ranges::view_interface<utf8_decode_view> {

    public:

        /**
         * Constructs a `decode_utf8_view` instance.
         *
         * @param bytes
         *   The bytes to decode.
         */

        constexpr explicit
        utf8_decode_view(std::span<const std::byte> bytes)
        noexcept:
            bytes_(bytes)
        {
            // empty
        }

        /**
         * Gets an iterator pointing at the beginning of the codepoint range.
         *
         * @return
         *   The iterator.
         */

        inline
        utf8_decode_iterator
        begin()
        const
        {
            const auto* data = bytes_.data();
            return utf8_decode_iterator(data, data + bytes_.size());
        }

        /**
         * Gets the view sentinel.
         *
         * @return
         *   The sentinel.
         */

        constexpr
        std::default_sentinel_t
        end()
        const noexcept
        {
            return std::default_sentinel;
        }

    private:

        std::span<const std::byte> bytes_;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::decode_utf8()
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Creates a `decode_utf8_view` that can be used to decode the given bytes.
     *
     * @param bytes
     *   The bytes to decode.
     *
     * @return
     *   The view.
     */

    export
    inline
    utf8_decode_view
    decode_utf8(std::span<const std::byte> bytes)
    {
        return utf8_decode_view(bytes);
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::utf8_string
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    std::string&&
    verify_utf8_string(std::string&& s)
    {
        std::ranges::for_each(
            decode_utf8(std::as_bytes(std::span(s))),
            [](unicode_codepoint) {});
        return std::move(s);
    }

    /**
     * Contains a valid UTF-8 encoded string.
     */

    export
    class utf8_string final: public string_proxy<utf8_string> {

    public:

        /**
         * Default constructor.
         */

        constexpr
        utf8_string() = default;

        /**
         * Constructs a `utf8_string` instance.
         *
         * @param args
         *   The arguments to pass to the `std::string` constructor.
         */

        template<class... Args>
        requires (std::constructible_from<std::string, Args...>)
        explicit (
            (sizeof...(Args) != 1) ||
            (
                ! implicitly_convertible_to<
                    boost::mp11::mp_front<boost::mp11::mp_list<Args...>>,
                    std::string
                >
            )
        )
        utf8_string(Args&&... args):
            string_proxy<utf8_string>(
                verify_utf8_string(std::string(std::forward<Args>(args)...)))
        {
            // empty
        }

    };

}
