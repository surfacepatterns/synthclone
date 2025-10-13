/**
 * @file
 *
 * Contains base functionality used by all components.
 */

module;

#include <synthclone/config.h>

// The forward declaration needs to be here so the declaration is not attached
// to the module.
class QWidget;

export module synthclone.core:component_core;

import std;

import synthclone.util;

import :metadata;
import :state;

///////////////////////////////////////////////////////////////////////////////
// synthclone::component_core_ops
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Contains core operations for a component type.
     *
     * @tparam T
     *   The base component type.
     */

    export
    template<class T>
    class component_core_ops: private nonmovable {

    public:

        /**
         * Destructs a `component_core_ops` instance.
         */

        virtual
        ~component_core_ops() = default;

        /**
         * Instantiates a new component instance.
         *
         * @return
         *   A pointer to the new component instance.
         */

        virtual
        std::unique_ptr<T>
        create() = 0;

    protected:

        /**
         * Constructs a `component_core_ops` instance.
         */

        explicit
        component_core_ops() = default;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::component_editor_ops
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Contains component editor operations.
     *
     * @tparam T
     *   The base component type.
     * @tparam M
     *   The message variant type.
     */

    export
    template<class T, class M>
    class component_editor_ops: private nonmovable {

    public:

        /**
         * Destructs a `component_editor_ops` instance.
         */

        virtual
        ~component_editor_ops() = default;

        /**
         * Populates the given window with an editor interface, allowing the
         * user to edit the component state, and manages the window throughout
         * the lifetime of the edit operations (e.g. until the window is
         * closed).
         *
         * @param component
         *   The component to be edited.
         * @param parent
         *   The widget to use as the parent of the edit interface.
         * @param stop_token
         *   A stop token that will be set if the operation is cancelled.
         *
         * @return
         *   A generator that is used to send messages back to the host.
         */

        virtual
        std::generator<M>
        edit(T& component, ::QWidget* parent, std::stop_token stop_token) = 0;

    protected:

        /**
         * Constructs a `component_editor_ops` instance.
         */

        explicit
        component_editor_ops() = default;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::component_state_ops
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Contains component state operations.
     *
     * @tparam T
     *   The base component type.
     */

    export
    template<class T>
    class component_state_ops: private nonmovable {

    public:

        /**
         * Destructs a `component_state_ops` instance.
         */

        virtual
        ~component_state_ops() = default;

        /**
         * Gets a snapshot of the given component's state.
         *
         * @param component
         *   The component to get the state for.
         *
         * @return
         *   The component state.
         */

        virtual
        state_value
        dump(const T& component) = 0;

        /**
         * Loads a component from the given state.
         *
         * @param state
         *   The state to load the component from.
         *
         * @return
         *   A pointer to the loaded component instance.
         */

        virtual
        std::unique_ptr<T>
        load(const state_value& state) = 0;

    protected:

        /**
         * Constructs a `component_state_ops` instance.
         */

        explicit
        component_state_ops() = default;

    };

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
         * The component external editor operations.
         */

        std::unique_ptr<E> external_editor_ops;

        /**
         * The component state operations.
         */

        std::unique_ptr<S> state_ops;

        /**
         * Metadata describing components of the given component type.
         */

        metadata_init_args metadata;

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
    verify_component_external_editor_ops(std::unique_ptr<E>&& editor_ops)
    {
        if constexpr(R) {
            verify(
                editor_ops != nullptr,
                "`external_editor_ops` cannot be set to null");
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
     * @tparam ExternalEditorOpsRequired
     *   Whether or not external editor operations are required for the
     *   component type.
     */

    export
    template<
        class C,
        class E,
        class S,
        bool ExternalEditorOpsRequired
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
        core_ops()
        const noexcept
        {
            return core_ops_;
        }

        /**
         * Gets the (possibly optional) external editor operations for the
         * component type.
         *
         * @return
         *   A pointer to the external editor operations.
         */

        constexpr
        const std::unique_ptr<E>&
        external_editor_ops()
        const noexcept
        {
            return external_editor_ops_;
        }

        /**
         * Gets the metadata corresponding to the component type.
         *
         * @return
         *   The metadata.
         */

        constexpr
        const metadata&
        metadata()
        const noexcept
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
        state_ops()
        const noexcept
        {
            return state_ops_;
        }

    protected:

        constexpr
        component_type(
            std::unique_ptr<C>&& core_ops,
            std::unique_ptr<E>&& external_editor_ops,
            std::unique_ptr<S>&& state_ops,
            metadata_init_args&& metadata
        ):
            metadata_(std::move(metadata)),
            core_ops_(verify_component_core_ops(std::move(core_ops))),
            external_editor_ops_(
                verify_component_external_editor_ops<
                    ExternalEditorOpsRequired
                >(std::move(external_editor_ops))),
            state_ops_(std::move(state_ops))
        {
            // empty
        }

    private:

        class metadata metadata_;

        std::unique_ptr<C> core_ops_;
        std::unique_ptr<E> external_editor_ops_;
        std::unique_ptr<S> state_ops_;

    };

}
