/**
 * @file
 *
 * Contains general utility functionality not specified by the standard
 * library.
 */

module;

#include <synthclone/config.h>

export module synthclone.util:utility;

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
     * The implementation of this class is meant to be compatible with the
     * class of the same name proposed for inclusion in the standard library:
     *
     * https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/p2895r0.html
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
     * The implementation of this class is meant to be compatible with the
     * class of the same name proposed for inclusion in the standard library:
     *
     * https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/p2895r0.html
     */

    export
    using nonmovable = nonmovable_::nonmovable;

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
