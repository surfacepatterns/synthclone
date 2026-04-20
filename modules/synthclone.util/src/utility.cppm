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

import :debug;

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
// synthclone::out_param
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Type passed to functions wherein an output parameter is expected and
     * cannot be transmitted via a return value (e.g. coroutine functions that
     * return `std::generator` types).
     */

    export
    template<class T>
    requires (std::movable<T> || std::copyable<T>)
    class out_param final: private nonmovable {

    public:

        /**
         * Checks whether or not a result has already been assigned to the
         * `out_param` instance.
         *
         * @return
         *   The boolean indicator.
         */

        constexpr explicit
        operator bool() const noexcept
        {
            return static_cast<bool>(result_);
        }

        /**
         * Assigns a result to the `out_param` instance.
         *
         * @param result
         *   The result to assign.
         *
         * @return
         *   This `out_param` instance.
         */

        constexpr
        out_param&
        operator=(T&& result)
        requires (std::movable<T>)
        {
            verify(! result_, "result is already set");
            result_ = std::move(result);
            return *this;
        }

        /**
         * Assigns a result to the `out_param` instance.
         *
         * @param result
         *   The result to assign.
         *
         * @return
         *   This `out_param` instance.
         */

        constexpr
        out_param&
        operator=(const T& result)
        requires (std::copyable<T>)
        {
            verify(! result_, "result is already set");
            result_ = result;
            return *this;
        }

        /**
         * Constructs a result in-place in the `out_param` instance.
         *
         * @param args
         *   The arguments to pass to the constructor.
         *
         * @return
         *   The constructed result.
         */

        template<class... Args>
        requires (std::constructible_from<T, Args&&...>)
        constexpr
        T&
        emplace(Args&&... args)
        {
            verify(! result_, "result is already set");
            return result_.emplace(std::forward<Args>(args)...);
        }

        /**
         * Constructs a result in-place in the `out_param` instance.
         *
         * @param ilist
         *   An initializer list argument to pass to the constructor.
         * @param args
         *   The arguments to pass to the constructor.
         *
         * @return
         *   The constructed result.
         */

        template<class U, class... Args>
        requires (
            std::constructible_from<T, std::initializer_list<U>&, Args&&...>
        )
        T&
        emplace(std::initializer_list<U> ilist, Args&&... args)
        {
            verify(! result_, "result is already set");
            return result_.emplace(ilist, std::forward<Args>(args)...);
        }

        /**
         * Gets the result stored in the `out_param` instance.
         *
         * @return
         *   The stored result.
         */

        constexpr
        T&
        value() &
        {
            verify(static_cast<bool>(result_), "result is not set");
            return *result_;
        }

        /**
         * Gets the result stored in the `out_param` instance.
         *
         * @return
         *   The stored result.
         */

        constexpr
        const T&
        value() const &
        {
            verify(static_cast<bool>(result_), "result is not set");
            return *result_;
        }

        /**
         * Gets the result stored in the `out_param` instance.
         *
         * @return
         *   The stored result.
         */

        constexpr
        T&&
        value() &&
        {
            verify(static_cast<bool>(result_), "result is not set");
            return *(std::move(result_));
        }

    private:

        std::optional<T> result_;

    };

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
