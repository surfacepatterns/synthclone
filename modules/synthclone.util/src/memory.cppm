/**
 * @file
 *
 * Contains functionality that might normally appear in the `memory` header in
 * the standard library, but isn't implemented there.
 */

module;

#include <synthclone/config.h>

export module synthclone.util:memory;

import std;

///////////////////////////////////////////////////////////////////////////////
// synthclone::pointer
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Checks whether a type has a corresponding `std::pointer_traits`
     * specialization.
     *
     * @tparam T
     *   The type to check.
     */

    export
    template<class T>
    concept pointer =
        requires (T& t) {
            typename std::pointer_traits<std::remove_cv_t<T>>::element_type;
            requires std::same_as<
                std::remove_cv_t<T>,
                typename std::pointer_traits<std::remove_cv_t<T>>::pointer
            >;

            { std::to_address(t) } ->
                std::same_as<
                    typename std::pointer_traits<
                        std::remove_cv_t<T>
                    >::element_type*
                >;
        };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::pointer_difference_t
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Gets the difference type for a given pointer type.
     *
     * @tparam T
     *   The pointer type.
     */

    export
    template<pointer T>
    using pointer_difference_t =
        typename std::pointer_traits<std::remove_cv_t<T>>::difference_type;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::pointer_element_t
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Gets the element type for a given pointer type.
     *
     * @tparam T
     *   The pointer type.
     */

    export
    template<pointer T>
    using pointer_element_t =
        typename std::pointer_traits<std::remove_cv_t<T>>::element_type;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::pointer_address_t
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Gets the *raw* pointer type for a given pointer type.
     *
     * @tparam T
     *   The pointer type.
     */

    export
    template<pointer T>
    using pointer_address_t = pointer_element_t<T>*;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::pointer_to
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Checks whether a type is a pointer to the given element type.
     *
     * @tparam T
     *   The type to check.
     * @tparam E
     *   The element type.
     */

    export
    template<class T, class E>
    concept pointer_to = pointer<T> && std::same_as<pointer_element_t<T>, E>;

}
