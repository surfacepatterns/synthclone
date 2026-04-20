/**
 * @file
 *
 * Contains base functionality used by all components.
 */

module;

#include <synthclone/config.h>

export module synthclone.core:component_core;

import std;

import synthclone.util;

import :metadata_core;
import :session;
import :state;

///////////////////////////////////////////////////////////////////////////////
// synthclone::component_metadata_init_args
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Used to initialize `component_metadata` instances using aggregate
     * initialization.
     */

    export
    struct component_metadata_init_args final {

        /**
         * The component identifier.
         */

        metadata_element identifier;

        /**
         * The component version.
         */

        metadata_element version;

        /**
         * The optional title for the component.
         */

        std::optional<metadata_element> title;

        /**
         * The optional URL that links to more information on the component.
         */

        std::optional<metadata_url> url;

        /**
         * The optional license descriptor for the component.
         */

        std::optional<metadata_element> license;

        /**
         * The creators of the component.
         */

        metadata_element_sequence creators;

        /**
         * The contributors to the component.
         */

        metadata_element_sequence contributors;

        /**
         * The description of the component.
         */

        metadata_text description;

        /**
         * The tiered category for the component.
         */

        metadata_element_sequence category;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::component_metadata
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Contains metadata for a component.
     */

    export
    class component_metadata final: public metadata {

    public:

        /**
         * Move constructor.
         */

        component_metadata(component_metadata&&) = default;

        /**
         * Copy constructor.
         */

        component_metadata(const component_metadata&) = default;

        /**
         * Constructs a `component_metadata` instance.
         *
         * @param args
         *   The data to use to populate the `metadata` instance.
         */

        constexpr
        component_metadata(component_metadata_init_args args) noexcept:
            metadata(
                std::move(args.identifier), std::move(args.version),
                std::move(args.title), std::move(args.url),
                std::move(args.license), std::move(args.creators),
                std::move(args.contributors), std::move(args.description)),
            category_(std::move(args.category))
        {
            // empty
        }

        /**
         * Move assignment operator.
         */

        component_metadata&
        operator=(component_metadata&&) = default;

        /**
         * Copy assignment operator.
         */

        component_metadata&
        operator=(const component_metadata& other)
        {
            *this = component_metadata(other);
            return *this;
        }

        /**
         * Gets the tiered category for the component.
         *
         * @return
         *   A range representing the tiered category.
         */

        constexpr
        auto
        category() const
        {
            return std::ranges::subrange(category_.cbegin(), category_.cend());
        }

    private:

        metadata_element_sequence category_;

    };

    /**
     * Gets a boolean indicating whether or not two `component_metadata`
     * instances contain exactly the same metadata.
     *
     * @param lhs
     *   The first instance.
     * @param rhs
     *   The second instance.
     *
     * @return
     *   The boolean indicator.
     */

    export
    constexpr
    bool
    operator==(const component_metadata& lhs, const component_metadata& rhs)
    {
        return equal(lhs, rhs) &&
            std::ranges::equal(lhs.category(), rhs.category());
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::component_type_init_args
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Common type used to initialize `component_type` instances using
     * aggregate initialization.
     *
     * @tparam C
     *   The core operations type.
     * @tparam E
     *   The edit operations type.
     * @tparam S
     *   The state operations type.
     */

    export
    template<class C, class E, class S>
    struct component_type_init_args final {

        /**
         * The component core operations.
         */

        std::unique_ptr<C> core_ops;

        /**
         * The component editor operations.
         */

        std::unique_ptr<E> editor_ops;

        /**
         * The component state operations.
         */

        std::unique_ptr<S> state_ops;

        /**
         * Metadata describing the given component type.
         */

        component_metadata_init_args metadata;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::component_type
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    template<class C>
    constexpr
    std::unique_ptr<C>&&
    verify_component_core_ops(std::unique_ptr<C>&& core_ops)
    {
        verify(core_ops != nullptr, "`core_ops` cannot be set to null");
        return std::move(core_ops);
    }

    template<bool R, class E>
    constexpr
    std::unique_ptr<E>&&
    verify_component_editor_ops(std::unique_ptr<E>&& editor_ops)
    {
        if constexpr(R) {
            verify(
                editor_ops != nullptr, "`editor_ops` cannot be set to null");
        }
        return std::move(editor_ops);
    }

    /**
     * Contains base operations and metadata corresponding to a specific
     * component type.
     *
     * @tparam C
     *   The core operations type.
     * @tparam E
     *   The edit operations type.
     * @tparam S
     *   The state operations type.
     * @tparam A
     *   The initialization arguments type.
     * @tparam EditorOpsRequired
     *   Whether or not editor operations are required for the component type.
     */

    export
    template<
        class C,
        class E,
        class S,
        bool EditorOpsRequired
    >
    class component_type: private noncopyable {

    public:

        /**
         * Gets the core operations for the component type.
         *
         * @return
         *   A pointer to the core operations.
         */

        constexpr
        const std::unique_ptr<C>&
        core_ops() const noexcept
        {
            return core_ops_;
        }

        /**
         * Gets the (possibly optional) editor operations for the component
         * type.
         *
         * @return
         *   A pointer to the editor operations.
         */

        constexpr
        const std::unique_ptr<E>&
        editor_ops() const noexcept
        {
            return editor_ops_;
        }

        /**
         * Gets the metadata corresponding to the component type.
         *
         * @return
         *   The metadata.
         */

        constexpr
        const component_metadata&
        metadata() const noexcept
        {
            return metadata_;
        }

        /**
         * Gets the optional state operations for the component type.
         *
         * @return
         *   A pointer to the state operations.
         */

        constexpr
        const std::unique_ptr<S>&
        state_ops() const noexcept
        {
            return state_ops_;
        }

    protected:

        constexpr
        component_type(
            std::unique_ptr<C>&& core_ops,
            std::unique_ptr<E>&& editor_ops,
            std::unique_ptr<S>&& state_ops,
            component_metadata_init_args&& metadata
        ):
            metadata_(std::move(metadata)),
            core_ops_(verify_component_core_ops(std::move(core_ops))),
            editor_ops_(
                verify_component_editor_ops<
                    EditorOpsRequired
                >(std::move(editor_ops))),
            state_ops_(std::move(state_ops))
        {
            // empty
        }

    private:

        component_metadata metadata_;

        std::unique_ptr<C> core_ops_;
        std::unique_ptr<E> editor_ops_;
        std::unique_ptr<S> state_ops_;

    };

}
