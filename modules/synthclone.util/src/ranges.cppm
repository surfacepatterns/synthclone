/**
 * @file
 *
 * Contains range oriented functionality not found in the standard library.
 */

module;

#include <synthclone/config.h>

export module synthclone.util:ranges;

import std;

///////////////////////////////////////////////////////////////////////////////
// synthclone::container_compatible_range
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Used in the definition of containers in constructing an input range R
     * when its range reference type must be convertible to T.
     *
     * @tparam R
     *   The range type.
     * @tparam T
     *   The element type.
     *
     * @note
     *   This concept is the same as the exposition-only concept with the same
     *   name in the standard library.
     */

    export
    template<class R, class T>
    concept container_compatible_range =
        std::ranges::input_range<R> &&
        std::convertible_to<std::ranges::range_reference_t<R>, T>;

}
