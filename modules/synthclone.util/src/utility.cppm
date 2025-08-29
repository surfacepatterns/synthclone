/**
 * @file
 *
 * Contains general utility functionality not specified by the standard
 * library.
 */

module;

#include <synthclone/config.h>

export module synthclone.util:utility;

import std;

///////////////////////////////////////////////////////////////////////////////
// synthclone::noncopyable
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE::noncopyable_ {

    export
    class noncopyable {

    public:

        noncopyable() = default;

        noncopyable(noncopyable&&) = default;

        noncopyable&
        operator=(noncopyable&&) = default;

    };

}

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Inherited type that prevents copying of instances of a given class.
     *
     * @note
     *   The implementation of this class is meant to be compatible with the
     *   class of the same name proposed for inclusion in the standard library:
     *
     *   https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/p2895r0.html
     */

    export
    using noncopyable = noncopyable_::noncopyable;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::nonmovable
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE::nonmovable_ {

    export
    class nonmovable {

    public:

        nonmovable() = default;

    private:

        nonmovable(const nonmovable&) = delete;

        nonmovable&
        operator=(const nonmovable&) = delete;

    };

}

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Inherited type that prevents copying/moving of instances of a given
     * class.
     *
     * @note
     *   The implementation of this class is meant to be compatible with the
     *   class of the same name proposed for inclusion in the standard library:
     *
     *   https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/p2895r0.html
     */

    export
    using nonmovable = nonmovable_::nonmovable;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::type_sequence
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Simple type that contains a parameter pack of zero or more types.
     *
     * @tparam T
     *   The types.
     */

    export
    template<class... T>
    struct type_sequence {};

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::make_type_sequence
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    template<class T, std::size_t N, class... Types>
    struct make_type_sequence_impl {

        using type = std::conditional_t<
            sizeof...(Types) == N,
            type_sequence<Types...>,
            typename make_type_sequence_impl<T, N, T, Types...>::type
        >;

    };

    /**
     * Type alias that calculates a type sequence containing N copies of the
     * given type.
     *
     * @tparam T
     *   The type.
     * @tparam N
     *   The number of copies.
     */

    export
    template<class T, std::size_t N>
    using make_type_sequence = make_type_sequence_impl<T, N>::type;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::validated_t
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Type used by functions to match the `validated` tag.
     */

    export
    struct validated_t {

        /**
         * Constructs a `validated_t` instance.
         */

        explicit
        validated_t() = default;

    };

    /**
     * Tag used to disambiguate functions where the passed arguments have
     * already been validated.
     *
     * The tag is generally used by a function that is a friend of a specific
     * class to call a constructor that is only accessible to the function.
     */

    export
    constexpr inline
    validated_t validated{};

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::to_signed()
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Converts the given integral to the corresponding signed integral type.
     *
     * @param n
     *   The argument to convert.
     *
     * @return
     *   The converted result.
     *
     * @note
     *   The implementation of this function is meant to be compatible with the
     *   function of the same name proposed for inclusion in the standard
     *   library:
     *
     *   https://eisenwave.github.io/cpp-proposals/to-signed-unsigned.html
     */

    export
    template<class T>
    constexpr
    std::make_signed_t<T>
    to_signed(T x) noexcept
    {
        return static_cast<std::make_signed_t<T>>(x);
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::to_unsigned()
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Converts the given integral to the corresponding unsigned integral type.
     *
     * @param n
     *   The argument to convert.
     *
     * @return
     *   The converted result.
     *
     * @note
     *   The implementation of this function is meant to be compatible with the
     *   function of the same name proposed for inclusion in the standard
     *   library:
     *
     *   https://eisenwave.github.io/cpp-proposals/to-signed-unsigned.html
     */

    export
    template<class T>
    constexpr
    std::make_unsigned_t<T>
    to_unsigned(T x) noexcept
    {
        return static_cast<std::make_unsigned_t<T>>(x);
    }

}
