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
