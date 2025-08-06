/**
 * @file
 *
 * Contains functionality that might normally appear in the `functional`
 * header in the standard library, but isn't implemented there.
 */

module;

#include <synthclone/config.h>

export module synthclone.util:functional;

import std;

///////////////////////////////////////////////////////////////////////////////
// synthclone::hashable
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Checks whether a type has a corresponding `std::hash` specialization.
     *
     * @tparam T
     *   The type to check.
     */

    export
    template<class T>
    concept hashable = requires (T obj) {
        { std::hash<T>{}(obj) } -> std::convertible_to<std::size_t>;
    };

}
