/**
 * @file
 *
 * Contains generic concepts not found in the standard library.
 */

module;

#include <synthclone/config.h>

export module synthclone.util:concepts;

import std;

///////////////////////////////////////////////////////////////////////////////
// synthclone::explicitly_convertible_to
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Checks if a type can be explicitly converted to another type.
     *
     * @tparam From
     *   The type to convert from.
     * @tparam To
     *   The type to convert to.
     */

    export
    template<class From, class To>
    concept explicitly_convertible_to = requires {
        static_cast<To>(std::declval<From>());
    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::implicitly_convertible_to
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Checks if a type can be implicitly converted to another type.
     *
     * @tparam From
     *   The type to convert from.
     * @tparam To
     *   The type to convert to.
     */

    export
    template<class From, class To>
    concept implicitly_convertible_to = std::is_convertible_v<From, To>;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::arithmetic
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Checks if a type is an arithmetic type.
     *
     * @tparam T
     *   The type to check.
     */

    export
    template<class T>
    concept arithmetic = std::is_arithmetic_v<T>;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::boolean_testable
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Checks if an expression is convertible to `bool` and has the usual
     * behavior with regards to logical operators.
     *
     * @tparam T
     *   The type to check.
     *
     * @note
     *   This concept is based on the exposition concept `boolean-testable`
     *   defined in the standard.
     */

    export
    template<class T>
    concept boolean_testable =
        std::convertible_to<T, bool> &&
        requires (T&& b) {
            { ! std::forward<T>(b) } -> std::convertible_to<bool>;
        };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::partially_ordered_with
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Checks if a type is partially ordered with another type.
     *
     * @tparam T
     *   The type to check.
     * @tparam U
     *   The type to check against.
     *
     * @note
     *   This concept is based on the exposition concept
     *   `__PartiallyOrderedWith` defined in the standard.
     */

    export
    template<class T, class U>
    concept partially_ordered_with =
        requires (
            const std::remove_reference_t<T>& t,
            const std::remove_reference_t<U>& u
        ) {
            { t < u } -> boolean_testable;
            { t > u } -> boolean_testable;
            { t <= u } -> boolean_testable;
            { t >= u } -> boolean_testable;
            { u < t } -> boolean_testable;
            { u > t } -> boolean_testable;
            { u <= t } -> boolean_testable;
            { u >= t } -> boolean_testable;
        };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::scalar
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Checks if a type is a scalar type.
     *
     * @tparam T
     *   The type to check.
     */

    export
    template<class T>
    concept scalar = std::is_scalar_v<T>;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::weakly_equality_comparable_with
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Checks if a type is a weakly equality comparable with another type.
     *
     * @tparam T
     *   The type to check.
     * @tparam U
     *   The type to check against.
     *
     * @note
     *   This concept is based on the exposition concept
     *   `__WeaklyEqualityComparableWith` defined in the standard.
     */

    export
    template<class T, class U>
    concept weakly_equality_comparable_with =
        requires (
            const std::remove_reference_t<T>& t,
            const std::remove_reference_t<U>& u
        ) {
            { t == u } -> boolean_testable;
            { t != u } -> boolean_testable;
            { u == t } -> boolean_testable;
            { u != t } -> boolean_testable;
        };

}
